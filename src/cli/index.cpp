#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <cctype>

#include "index.h"

#include "../constants/index.h"
#include "../helpers/index.h"

using namespace std;
using namespace commandInterface;
using namespace errors::CLI;
using namespace Json;

CLI::CLI(int argc, char *argv[]) {
    if (this->isCommand(argv[1])) {
        if (string(argv[1]) == string(COMMANDS::RUN)) this->getEnvFromConfig(argv[2]);
        else if (string(argv[1]) == string(COMMANDS::LIST)) this->listRunnerProcess();
        else this->reject((string(argv[1]) + "  " + string(NO_COMMAND)).c_str());
    } else {
        for (int i = 1; i < argc; i++) {
            this->getEnvFromCLI(argv[i], false);
        }
        if (strlen(this->env.command.c_str()) == 0) this->reject(NO_COMMAND);
    }
}

string CLI::parseCandidate(const char *candidate, const char *needCandidate) {
    return string(candidate).substr(strlen(needCandidate)+1, strlen(string(candidate).c_str()));
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

    drawTableHeader();

    if (processes.good()) {
        string line;

        while (getline(processes, line)) {
            vector<string> splitted = helpers::split(line, "|");
            cout << "test are: " << line << endl;

            drawTableBody(splitted);
        }
    }

    processes.close();
}

ENV CLI::getEnv() {
    return this->env;
}

void CLI::getEnvFromCLI(const char *candidate, bool default_foreground) {
    this->env.isForeground = default_foreground;

    if (this->isCandidate(candidate, FLAGS::WATCH_PATH)) {
        this->env.path_to_watch = this->parseCandidate(candidate, FLAGS::WATCH_PATH);
    } else if (this->isCandidate(candidate, FLAGS::WATCH_FILE)) {
        this->env.path_to_watch_file = this->parseCandidate(candidate, FLAGS::WATCH_FILE);
    } else if (this->isCandidate(candidate, FLAGS::EXEC)) {
        this->env.command = this->parseCandidate(candidate, FLAGS::EXEC);
    } else if (this->isCandidate(candidate, FLAGS::CONFIG)) {
        this->env.path_to_config = this->parseCandidate(candidate, FLAGS::CONFIG);
    } else if (this->isCandidate(candidate, FLAGS::PORT)) {
        this->env.port = this->parseCandidate(candidate, FLAGS::PORT);
    } else {
        if (candidate == FLAGS::FOREGROUND) {
            this->env.isForeground = true;
        } else {
            this->reject((string(candidate) + " " + string(UNKNOWN_COMMAND)).c_str());
        }
    }
}

void CLI::getEnvFromConfig(const char *path_to_env) {
    ifstream file(path_to_env);

    this->env.isForeground = true;

    if (file.good()) {
        Reader reader;
        Value  json;

        reader.parse(file, json);

        Value::Members jsonENV = json.getMemberNames();

        for (auto &envName: jsonENV) {
            if (envName == configInterface::CONFIG::EXEC) this->env.command = json[envName].asString();
            if (envName == configInterface::CONFIG::WATCH_FILE) this->env.path_to_watch_file = json[envName].asString();
            if (envName == configInterface::CONFIG::WATCH_PATH) this->env.path_to_watch = json[envName].asString();
            if (envName == configInterface::CONFIG::CONFIG) this->env.path_to_config = json[envName].asString();
            if (envName == configInterface::CONFIG::PORT) this->env.port= json[envName].asString();
            if (envName == configInterface::CONFIG::FOREGROUND) this->env.isForeground = json[envName].asBool();
        }
    } else {
        this->reject(NO_CONFIG);
    }
}