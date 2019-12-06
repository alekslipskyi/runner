#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <thread>

#include "index.h"

#include "../helpers/index.h"
#include "../table/index.h"

using namespace std;
using namespace commandInterface;
using namespace errors::CLI;
using namespace Json;

string CLI::parseCandidate(const char *candidate, const char *needCandidate) {
    return string(candidate).substr(strlen(needCandidate)+1, strlen(string(candidate).c_str()));
}

void CLI::onListenCommand(CLI *self) {
    auto command = string();

    cin >> command;

    if (command == COMMANDS_FOREGROUND::STOP) self->stop();
    else if (command == COMMANDS_FOREGROUND::RESUME) self->resume();
    else if (command == COMMANDS_FOREGROUND::RELOAD) self->reload();
    else system(command.c_str());

    self->addListenCommand();
}

void CLI::addListenCommand() {
    thread command(CLI::onListenCommand, this);
    command.detach();
}

bool CLI::isCandidate(const char* candidate, const char *expectedCandidate) {
    return string(candidate).substr(0, strlen(expectedCandidate)) == string(expectedCandidate);
}

bool CLI::isCommand(const char *first_argument) {
    vector<const char*> commands = { COMMANDS::RUN, COMMANDS::LIST };
    return helpers::includes<vector<const char *>, const char *>(commands, first_argument);
}

void CLI::reject(const char *errMessage) {
    cout << errMessage << endl;
    exit(0);
}

void CLI::listRunnerProcess() {
    ifstream processes(CORE::TMP::PUB_FILE);

    if (processes.good()) {
        string lineHeader;
        vector<string> data;
        while (getline(processes, lineHeader)) {
            vector<string> splitted = helpers::split(lineHeader, "|");
            data.push_back(lineHeader);
        }

        Table table(data);
    } else {
        cout << "processes are not exist" << endl;
    }

    processes.close();
}

ENV CLI::getEnv(int argc, char *argv[]) {
    auto env = ENV();

    if (CLI::isCommand(argv[1])) {
        if (string(argv[1]) == string(COMMANDS::RUN)) env = CLI::getEnvFromConfig(argv[2]);
        else if (string(argv[1]) == string(COMMANDS::LIST)) CLI::listRunnerProcess();
        else CLI::reject((string(argv[1]) + "  " + string(NO_COMMAND)).c_str());
    } else {
        env.command = argv[1];
        env.path_to_watch = argv[2];
        env.fileType = argv[3];

        for (int i = 4; i < argc; i++) {
            CLI::getEnvFromCLI(argv[i], true, &env);
        }
        if (strlen(env.command.c_str()) == 0) CLI::reject(NO_COMMAND);
    }

    return env;
}

void CLI::getEnvFromCLI(const char *candidate, bool default_foreground, ENV *env) {
    env->isForeground = default_foreground;

    if (CLI::isCandidate(candidate, FLAGS::CONFIG)) {
        env->path_to_config = CLI::parseCandidate(candidate, FLAGS::CONFIG);
    } else if (CLI::isCandidate(candidate, FLAGS::PORT)) {
        env->port = CLI::parseCandidate(candidate, FLAGS::PORT);
    } else if (CLI::isCandidate(candidate, FLAGS::ENV_FILE)) {
        env->path_to_env_file = CLI::parseCandidate(candidate, FLAGS::ENV_FILE);
    } else if (CLI::isCandidate(candidate, FLAGS::DEBUG)) {
        env->isDebug = true;
    } else {
        if (candidate == FLAGS::FOREGROUND) {
            env->isForeground = true;
        } else {
            CLI::reject((string(candidate) + " " + string(UNKNOWN_COMMAND)).c_str());
        }
    }
}

ENV CLI::getEnvFromConfig(const char *path_to_env) {
    auto env = ENV();

    ifstream file(path_to_env);

    env.isForeground = true;

    if (file.good()) {
        Reader reader;
        Value  json;

        reader.parse(file, json);

        Value::Members jsonENV = json.getMemberNames();

        for (auto &envName: jsonENV) {
            if (envName == configInterface::CONFIG::EXEC) env.command = json[envName].asString();
            if (envName == configInterface::CONFIG::FILE_TYPE) env.fileType = json[envName].asString();
            if (envName == configInterface::CONFIG::WATCH_PATH) env.path_to_watch = json[envName].asString();
            if (envName == configInterface::CONFIG::CONFIG) env.path_to_config = json[envName].asString();
            if (envName == configInterface::CONFIG::PORT) env.port= json[envName].asString();
            if (envName == configInterface::CONFIG::FOREGROUND) env.isForeground = json[envName].asBool();
        }
    } else {
        CLI::reject(NO_CONFIG);
    }

    return env;
}