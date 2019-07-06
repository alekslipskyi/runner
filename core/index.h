#ifndef RUNNER_CORE_H
#define RUNNER_CORE_H

#include <string>
#include <chrono>

#include "../filewatcher/index.h"

using namespace std;

enum errors { NO_COMMAND, NO_PATH_TO_WATCH };
enum enumCandidates { watchpath, watchfile, forcekill, exec, config };

const string errMessages[] = {
        "please specify the command, alike --exec=\"ls\"",
        "env or arg PATH_TO_WATCH is required, alike --watch=./",
};
const string candidates[] = { "--watchpath", "--watchfile", "--forcekill", "--exec", "--config" };

class Core {
private:
    string      command;
    string      path_to_config;
    string      pid;
    string      path_to_watch;
    string      path_to_watch_file;
    void        recognizePID(string pid);
    bool        is_force_kill;
    void        error(string message);
    void        createProcess();
    void        reloadProcess(string path_to_watch, FileStatus status);
    void        findCandidate(string candidate);
    bool        isCandidate(string candidate, int expectedCandidate);
    string      parseCandidate(string candidate, int needCandidate);
    FileWatcher *fileWatcher;
public:
    void        jsonToEnv();
    void        exec();
    Core(int argc, char *argv[]);
};


#endif //RUNNER_CORE_H
