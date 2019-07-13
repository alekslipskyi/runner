#ifndef RUNNER_CORE_H
#define RUNNER_CORE_H

#include <string>
#include <chrono>

#include "../filewatcher/index.h"
#include "../constants/index.h"

using namespace std;

class Core {
private:
    ENV         env;
    void        ensurePortIsFree();
    void        createProcess();
    void        reloadProcess(const string* path_to_watch, FileStatus status);
    FileWatcher *fileWatcher;
public:
    void        jsonToEnv();
    void        exec();
    Core(ENV *env);
    void        putToBackground(int argc, char *__arguments[]);
};


#endif //RUNNER_CORE_H
