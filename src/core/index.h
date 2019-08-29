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
    bool        isReload;
    void        ensurePortIsFree();
    void        createProcess();
    void        reloadProcess(const string* path_to_watch, FileStatus status, bool forceReload);
    void        saveBackgroundTask(pid_t pid);
    FileWatcher *fileWatcher;
protected:
    void        jsonToEnv();
    void        exec();
    void        fileTOEnv();
    void        stop();
    void        resume();
    void        reload();
    void        putToBackground(int argc, char *__arguments[]);
public:
    explicit    Core(ENV env);
};


#endif //RUNNER_CORE_H
