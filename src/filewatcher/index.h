#ifndef FILEWATCHER_H_
#define FILEWATCHER_H_

#include <chrono>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <thread>
#include <filesystem>

using namespace std;

enum class FileStatus {created, modified, erased};

class FileWatcher {
private:
    unordered_map<string, filesystem::file_time_type> paths_;
    chrono::duration<int, milli>    delay;
    bool                            running_;
    bool                            contains(const string &key);
    string                          path_to_watch;
    string                          path_to_file;
    filesystem::file_time_type      current_file_last_write_time;
public:
    FileWatcher(string path_to_watch, string path_to_file, chrono::duration<int, milli> delay);
    void                            listenPath(const std::function<void (string, FileStatus)> &action);
    void                            listenFile(const std::function<void (string, FileStatus)> &action);
};

#endif
