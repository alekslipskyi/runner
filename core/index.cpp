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
        this->findCandidate(string(argv[i]));
    }

    if (strlen(this->path_to_watch.c_str()) == 0 && strlen(this->path_to_watch_file.c_str()) == 0) {
        this->error(errMessages[errors::NO_PATH_TO_WATCH]);
    }

    if (strlen(this->command.c_str()) == 0) {
        this->error(errMessages[errors::NO_COMMAND]);
    }

    if (strlen(this->path_to_watch.c_str()) != 0) {
        this->fileWatcher = new FileWatcher(this->path_to_watch, "", chrono::milliseconds(500));
    } else if (strlen(this->path_to_watch_file.c_str()) != 0) {
        this->fileWatcher = new FileWatcher("", this->path_to_watch_file, chrono::milliseconds(500));
    }
}

void Core::error(string message) {
    cout << "Error: " << message.c_str() << endl;
    exit(0);
}

bool Core::isCandidate(string candidate, int expectedCandidate) {
    return candidate.substr(0, strlen(candidates[expectedCandidate].c_str())) == candidates[expectedCandidate];
}

string Core::parseCandidate(string candidate, int needCandidate) {
    return candidate.substr(strlen(candidates[needCandidate].c_str())+1, strlen(candidate.c_str()));
}

void Core::findCandidate(string candidate) {
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

    if (this->isCandidate(candidate, enumCandidates::forcekill)) {
        this->is_force_kill = true;
    }
}

void Core::jsonToEnv() {
    ifstream file(string(this->path_to_config));

    Reader reader;
    Value json;

    reader.parse(file, json);

    Value::Members members = json.getMemberNames();

    for (int i = 0; i < members.size(); i++) {
        setenv(members[i].c_str(), json[members[i]].asString().c_str(), 1);
    }
}

void Core::createProcess() {
    if (strlen(this->path_to_config.c_str()) != 0) {
        this->jsonToEnv();
    }

    int _pid;

    char _command[this->command.length()];

    strcpy(_command, this->command.c_str());

    char *argv[] = {executor, arguments, _command, nullptr};
    posix_spawn(&_pid, "/bin/sh", nullptr, nullptr, argv, environ);
    this->recognizePID(to_string(_pid));
}

void Core::recognizePID(string _pid) {
    ifstream tmpPID("/tmp/runner_pid.txt");

    system(("pgrep -P " + _pid + " > /tmp/runner_pid.txt").c_str());

    this_thread::sleep_for(chrono::milliseconds(800));

    string childPid;
    string sLine;

    getline(tmpPID, sLine);

    if (strlen(sLine.c_str()) != 0) {
        this->pid = sLine;
        this->recognizePID(sLine);
    }
}

void Core::reloadProcess(string path_to_watch, FileStatus status) {
    if(strlen(this->path_to_watch.c_str()) != 0 &&
       !filesystem::is_regular_file(filesystem::path(path_to_watch)) && status != FileStatus::erased) {
        return;
    }

    if (this->is_force_kill) {
        cout << "pid are" << this->pid << endl;
        system(("kill -9" + this->pid).c_str());
    }

    this->createProcess();
}

void Core::exec() {
    this->createProcess();

    if (strlen(this->path_to_watch.c_str()) == 0) {
        this->fileWatcher->listenFile([&] (string path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(path_to_watch, status);
        });
    } else {
        this->fileWatcher->listenPath([&] (string path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(path_to_watch, status);
        });
    }
}