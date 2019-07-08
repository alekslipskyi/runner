#ifndef RUNNER_CORE_H
#define RUNNER_CORE_H

#include <string>
#include <chrono>

#include "../filewatcher/index.h"

using namespace std;

enum errors { NO_COMMAND, NO_PATH_TO_WATCH, NO_CONFIG };
enum enumCandidates { watchpath, watchfile, port, exec, config, foreground };

const string errMessages[] = {
        "please specify the command, alike --exec=\"ls\"",
        "env or arg PATH_TO_WATCH is required, alike --watch=./",
        "config file are not specified like \"runner runner.conf\" or file are default file runner.conf not exist"
};
const string candidates[] = { "--watchpath", "--watchfile", "--port", "--exec", "--config", "--foreground" };

class Core {
private:
    string      command;
    bool        isForeground;
    string      path_to_config;
    string      path_to_watch;
    string      path_to_watch_file;
    string      port;
    void        ensurePortIsFree();
    void        error(const string* message);
    void        createProcess();
    void        reloadProcess(const string* path_to_watch, FileStatus status);
    void        findCandidate(const char* candidate);
    bool        isCandidate(const char* candidate, int expectedCandidate);
    string      parseCandidate(const char* candidate, int needCandidate);
    FileWatcher *fileWatcher;
public:
    void        jsonToEnv();
    void        exec();
    Core(int argc, char *argv[]);
    bool        isForegroundProcess();
    void        putToBackground(int argc, char *__arguments[]);
};


#endif //RUNNER_CORE_H
