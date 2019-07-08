#include <stdio.h>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <spawn.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "index.h"

extern char **environ;

using namespace std;
using namespace Json;

char* executor = (char *)"sh";
char* arguments = (char *)"-c";

Core::Core(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        this->findCandidate(argv[i]);
    }

    if (strlen(this->path_to_watch.c_str()) == 0 && strlen(this->path_to_watch_file.c_str()) == 0) {
        this->error(&errMessages[errors::NO_PATH_TO_WATCH]);
    }

    if (strlen(this->command.c_str()) == 0) {
        this->error(&errMessages[errors::NO_COMMAND]);
    }

    if (strlen(this->path_to_watch.c_str()) != 0) {
        this->fileWatcher = new FileWatcher(this->path_to_watch, "", chrono::milliseconds(500));
    } else if (strlen(this->path_to_watch_file.c_str()) != 0) {
        this->fileWatcher = new FileWatcher("", this->path_to_watch_file, chrono::milliseconds(500));
    }
}

void Core::error(const string* message) {
    cout << "Error: " << message->c_str() << endl;
    exit(0);
}

bool Core::isCandidate(const char* candidate, int expectedCandidate) {
    return string(candidate).substr(0, strlen(candidates[expectedCandidate].c_str())) == candidates[expectedCandidate];
}

string Core::parseCandidate(const char* candidate, int needCandidate) {
    return string(candidate).substr(strlen(candidates[needCandidate].c_str())+1, strlen(string(candidate).c_str()));
}

void Core::findCandidate(const char* candidate) {
    if (this->isCandidate(candidate, enumCandidates::watchpath)) {
        this->path_to_watch = this->parseCandidate(candidate, enumCandidates::watchpath);
    }

    if (this->isCandidate(candidate, enumCandidates::watchfile)) {
        this->path_to_watch_file = this->parseCandidate(candidate, enumCandidates::watchfile);
    }

    if (this->isCandidate(candidate, enumCandidates::exec)) {
        this->command = this->parseCandidate(candidate, enumCandidates::exec);
    }

    if (this->isCandidate(candidate, enumCandidates::config)) {
        this->path_to_config = this->parseCandidate(candidate, enumCandidates::config);
    }

    if (this->isCandidate(candidate, enumCandidates::port)) {
        this->port = this->parseCandidate(candidate, enumCandidates::port);
    }

    this->isForeground = !this->isCandidate(candidate, enumCandidates::foreground);
}

void Core::jsonToEnv() {
    ifstream file(string(this->path_to_config));

    Reader reader;
    Value json;

    reader.parse(file, json);

    Value::Members members = json.getMemberNames();

    for (auto &member : members) {
        setenv(member.c_str(), json[member].asString().c_str(), 1);
    }
}

void Core::createProcess() {
    if (strlen(this->port.c_str()) != 0) this->ensurePortIsFree();
    this_thread::sleep_for(chrono::milliseconds(300));

    if (strlen(this->path_to_config.c_str()) != 0) {
        this->jsonToEnv();
    }

    int _pid;

    char _command[this->command.length()];

    strcpy(_command, this->command.c_str());

    char *argv[] = {executor, arguments, _command, nullptr};
    posix_spawn(&_pid, "/bin/sh", nullptr, nullptr, argv, environ);
}

void Core::ensurePortIsFree() {
    system(("kill $(lsof -t -i :"+this->port+")").c_str());
}

void Core::reloadProcess(const string* _path_to_watch, FileStatus status) {
    if(strlen(this->path_to_watch.c_str()) != 0 &&
       !filesystem::is_regular_file(filesystem::path(*_path_to_watch)) && status != FileStatus::erased) {
        return;
    }

    this->createProcess();
}

bool Core::isForegroundProcess() {
    return this->isForeground;
}

void Core::putToBackground(int argc, char *__arguments[]) {
    pid_t pid;
    string stringCommand;

    for (int i = 0; i < argc; i++) {
        stringCommand += (string)"\"" + __arguments[i] + (string)"\"" + (string)" ";
    }

    stringCommand += " --foreground";

    char __command[stringCommand.length()];

    strcpy(__command, stringCommand.c_str());

    char *argv[] = {executor, arguments, __command, nullptr};
    posix_spawn(&pid, "/bin/sh", nullptr, nullptr, argv, environ);
}

void Core::exec() {
    this->createProcess();

    if (strlen(this->path_to_watch.c_str()) == 0) {
        this->fileWatcher->listenFile([&] (string _path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(&_path_to_watch, status);
        });
    } else {
        this->fileWatcher->listenPath([&] (string _path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(&_path_to_watch, status);
        });
    }
}