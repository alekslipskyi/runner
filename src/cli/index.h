#ifndef RUNNER_INDEX_H
#define RUNNER_INDEX_H

#include <string>
#include "../constants/index.h"

using namespace std;

class CLI {
private:
    bool        isFromConfig(const char *first_argument);
    bool        isCandidate(const char* candidate, const char *expectedCandidate);
    string      parseCandidate(const char *candidate, const char *needCandidate);
    void        getEnvFromCLI(const char *candidate, bool default_foreground);
    void        getEnvFromConfig(const char* path_to_env);
    void        reject(const char* errMessage);
public:
    CLI(int argc, char* argv[]);
    ENV         env;
    ENV         getEnv();
};

#endif //RUNNER_INDEX_H
