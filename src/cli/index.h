#ifndef RUNNER_INDEX_H
#define RUNNER_INDEX_H

#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include "../constants/index.h"
#include "../core/index.h"

using namespace std;

class CLI: public Core {
private:
    static bool        isCommand(const char *first_argument);
    static bool        isCandidate(const char* candidate, const char *expectedCandidate);
    static void        getEnvFromCLI(const char *candidate, bool default_foreground, ENV *env);
    static ENV         getEnvFromConfig(const char* path_to_env);
    static void        reject(const char* errMessage);
    static string      parseCandidate(const char *candidate, const char *needCandidate);
    static void        listRunnerProcess();
public:
    static void        onListenCommand(CLI *self);
    static ENV         getEnv(int argc, char *argv[]);
    void               addListenCommand();
    CLI(ENV env, int argc, char *argv[]): Core(std::move(env)) {
        this->addListenCommand();
        this->exec();
    };
};

#endif //RUNNER_INDEX_H
