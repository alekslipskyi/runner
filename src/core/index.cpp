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

Core::Core(ENV *env) {
    this->env = *env;
    if (strlen(this->env.path_to_watch.c_str()) != 0) {
        this->fileWatcher = new FileWatcher(this->env.path_to_watch, "", chrono::milliseconds(500));
    } else if (strlen(this->env.path_to_watch_file.c_str()) != 0) {
        this->fileWatcher = new FileWatcher("", this->env.path_to_watch_file, chrono::milliseconds(500));
    }
}

void Core::jsonToEnv() {
    ifstream file(string(this->env.path_to_config));

    Reader reader;
    Value json;

    reader.parse(file, json);

    Value::Members members = json.getMemberNames();

    for (auto &member : members) {
        setenv(member.c_str(), json[member].asString().c_str(), 1);
    }
}

void Core::createProcess() {
    if (strlen(this->env.port.c_str()) != 0) this->ensurePortIsFree();
    this_thread::sleep_for(chrono::milliseconds(300));

    if (strlen(this->env.path_to_config.c_str()) != 0) {
        this->jsonToEnv();
    }

    int _pid;

    char _command[this->env.command.length()];

    strcpy(_command, this->env.command.c_str());

    char *argv[] = {executor, arguments, _command, nullptr};
    posix_spawn(&_pid, "/bin/sh", nullptr, nullptr, argv, environ);
}

void Core::ensurePortIsFree() {
    system(("kill $(lsof -t -i :"+this->env.port+")").c_str());
}

void Core::reloadProcess(const string* _path_to_watch, FileStatus status) {
    cout << "hello" << endl;

    if(strlen(this->env.path_to_watch.c_str()) != 0 &&
       !filesystem::is_regular_file(filesystem::path(*_path_to_watch)) && status != FileStatus::erased) {
        return;
    }

    this->createProcess();
}

void Core::putToBackground(int argc, char *__arguments[]) {
    pid_t pid;
    string stringCommand;

    for (int i = 0; i < argc; i++) {
        stringCommand += (string)"\"" + __arguments[i] + (string)"\"" + (string)" ";
    }

    stringCommand += " --foreground > /dev/null";

    char __command[stringCommand.length()];

    strcpy(__command, stringCommand.c_str());

    char *argv[] = {executor, arguments, __command, nullptr};
    posix_spawn(&pid, "/bin/sh", nullptr, nullptr, argv, environ);
}

void Core::exec() {
    this->createProcess();

    if (strlen(this->env.path_to_watch.c_str()) == 0) {
        this->fileWatcher->listenFile([&] (string _path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(&_path_to_watch, status);
        });
    } else {
        this->fileWatcher->listenPath([&] (string _path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(&_path_to_watch, status);
        });
    }
}