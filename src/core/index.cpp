#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <spawn.h>
#include <cstring>
#include <csignal>
#include <thread>
#include <utility>
#include <regex>
#include <random>

#include "../helpers/index.h"
#include "index.h"
#include "../cli/index.h"

extern char **environ;

using namespace std;
using namespace Json;

char* executor = (char *)"sh";
char* arguments = (char *)"-c";

Core::Core(ENV env) {
    this->env = std::move(env);
    this->fileWatcher = new FileWatcher(chrono::milliseconds(500));
    this->isReload = true;
}

void Core::jsonToEnv() {
    ifstream file(string(this->env.path_to_config));
    if (file.good()) {
        Reader reader;
        Value json;

        reader.parse(file, json);

        Value::Members members = json.getMemberNames();

        for (auto &member : members) {
            setenv(member.c_str(), json[member].asString().c_str(), 1);
        }
    }
}

void Core::fileTOEnv() {
    ifstream envFILE(this->env.path_to_env_file);

    if (envFILE.good()) {
        string line;

        while(getline(envFILE, line)) {
            vector<string> __env = helpers::split(line, "=");
            setenv(__env[0].c_str(), __env[1].c_str(), 1);
        }
    } else {
        cout << "env file is invalid" << endl;
        exit(1);
    }
}

int Core::generateIDProcess() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1, 100000);

    return dist(mt);
}

void Core::destroyProcessFile() {
    auto fileName = ("/tmp/runner_output_" + to_string(this->processID)).c_str();

    ifstream fileProcess(fileName);

    if (fileProcess.good()) remove(fileName);
}

void Core::createProcessFile(const string &fileName) {
    ofstream processFile("/tmp/" + fileName);
    processFile.close();
}

void Core::putToOutLogs(const string& fileName) {
    auto lastIteration = 0;

    while (true) {
        ifstream processFile("/tmp/" + fileName);

        auto currentIteration = 0;
        string line;

        while(getline(processFile, line)) {
            currentIteration++;
            if (currentIteration > lastIteration) cout << line << endl;
            if (line.find("[break]") != string::npos) {
                while (cin.get() != '\n') {}
            }

            this_thread::sleep_for(chrono::milliseconds(50));
        }

        this_thread::sleep_for(chrono::milliseconds(150));
        lastIteration = currentIteration;
    }
}

void Core::createProcess() {
    if (strlen(this->env.port.c_str())) this->ensurePortIsFree();
    if (this->processID) this->destroyProcessFile();

    auto _processID = Core::generateIDProcess();
    auto fileName = "runner_output_" + to_string(_processID);

    Core::createProcessFile(fileName);

    this_thread::sleep_for(chrono::milliseconds(300));

    if (strlen(this->env.path_to_config.c_str()) != 0) this->jsonToEnv();

    int _pid;

    char _command[this->env.command.length()];

    strcpy(_command, (this->env.command + " > /tmp/" + fileName).c_str());

    char *argv[] = {executor, arguments, _command, nullptr};
    posix_spawn(&_pid, "/bin/sh", nullptr, nullptr, argv, environ);

    this->putToOutLogs(fileName);
    this->processID = _processID;
}

void Core::ensurePortIsFree() {
    system(("lsof -t -i :"+this->env.port+" > "+ CONSTANTS::CORE::TMP::LSOF_TMP +"").c_str());

    ifstream lsof_tmp(CONSTANTS::CORE::TMP::LSOF_TMP);

    if (lsof_tmp.good()) {
        string line;

        while(getline(lsof_tmp, line)) kill(atoi(line.c_str()), SIGKILL);

    } else {
        cout << "something went wrong" << endl;
        exit(1);
    }
}

void Core::reloadProcess(const string* _path_to_watch, FileStatus status, bool forceReload) {
    if (!this->isReload && !forceReload) return;

    string filePath = filesystem::path(*_path_to_watch);
    vector<string> fileTypes = helpers::split(env.fileType, ",");

    if (fileTypes[0] != "*") {
        if (helpers::every<vector<string>, string>(fileTypes, [&] (const string& type) -> bool {
            regex match("(.*)." + type);
            return !regex_match(filePath, match);
        })) {
            return;
        }
    }

    if(strlen(this->env.path_to_watch.c_str()) &&
       !filesystem::is_regular_file(filePath) &&
       status != FileStatus::erased && !forceReload) {
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

void Core::stop() {
    this->isReload = false;
}

void Core::resume() {
    this->isReload = true;
}

void Core::reload() {
    this->reloadProcess(&this->env.path_to_watch, FileStatus::modified, true);
}

void Core::exec() {
    if (strlen(this->env.path_to_env_file.c_str())) {
        Core::fileTOEnv();
        this->fileWatcher->addListenChange(this->env.path_to_env_file, [&] (const string& _path_to_watch, FileStatus status) -> void {
            Core::fileTOEnv();
            Core::reloadProcess(&this->env.path_to_env_file, FileStatus::modified, false);
        });
    }

    if (strlen(this->env.path_to_watch.c_str()) != 0) {
        this->fileWatcher->addListenChange(this->env.path_to_watch, [&] (const string& _path_to_watch, FileStatus status) -> void {
            Core::reloadProcess(&_path_to_watch, status, false);
        });
    }

    this->createProcess();
    this->fileWatcher->startListen();
}