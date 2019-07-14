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
    if (strlen(this->env.port.c_str())) this->ensurePortIsFree();
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
    FILE *tmp_lsof = tmpfile();

    cout << "test are" << read_symlink(filesystem::path("/proc/self/fd") / std::to_string(fileno(tmp_lsof))) << endl;

    system(("lsof -t -i :" + this->env.port + " > /tmp/runner_lsof.out").c_str());

    string line;

    char result[1];
    fgets(result, sizeof result, tmp_lsof);

    if (strlen(result)) {
        system(("kill $(lsof -t -i :"+this->env.port+")").c_str());
    }

    fclose(tmp_lsof);
}

void Core::reloadProcess(const string* _path_to_watch, FileStatus status) {
    cout << "hello" << endl;

    if(strlen(this->env.path_to_watch.c_str()) &&
       !filesystem::is_regular_file(filesystem::path(*_path_to_watch)) && status != FileStatus::erased) {
        return;
    }

    this->createProcess();
}

void Core::saveBackgroundTask(pid_t pid) {
    ofstream pub_runner_tasks(CORE::TMP::PUB_FILE);
    string _path_to_watch = this->env.path_to_watch;
    if (!strlen(_path_to_watch.c_str())) _path_to_watch = CORE::DEFAULT_VALUE_ENV;

    string _path_to_config = this->env.path_to_config;
    if (!strlen(_path_to_config.c_str())) _path_to_config = CORE::DEFAULT_VALUE_ENV;

    string _path_to_watch_file = this->env.path_to_config;
    if (!strlen(_path_to_watch_file.c_str())) _path_to_watch_file = CORE::DEFAULT_VALUE_ENV;

    string _port = this->env.port;
    if (!strlen(_port.c_str())) _port = CORE::DEFAULT_VALUE_ENV;

    pub_runner_tasks
    << random() << "|"
    << pid << "|"
    << this->env.command << "|"
    << _path_to_watch << "|"
    << _path_to_config << "|"
    << _path_to_watch_file << "|"
    << _port << endl;

    pub_runner_tasks.close();
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
    this->saveBackgroundTask(pid);
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