#ifndef FILEWATCHER_H_
#define FILEWATCHER_H_

#include <chrono>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <thread>
#include <experimental/filesystem>
#include <vector>

using namespace std;
using namespace experimental;

enum class FileStatus {created, modified, erased};

class FileWatcher {
private:
    unordered_map<string, filesystem::file_time_type> paths_;
    chrono::duration<int, milli>    delay;
    vector<std::function<void ()>>  events;
    bool                            running_;
    bool                            contains(const string &key);
    string                          path_to_watch;
    string                          path_to_file;
    filesystem::file_time_type      current_file_last_write_time;
    void                            addListenFile(string _path_to_watch, const std::function<void (string, FileStatus)> &action);
    void                            addListenPath(string _path_to_file, const std::function<void (string, FileStatus)> &action);
public:
    FileWatcher(chrono::duration<int, milli> delay);
    void                            startListen();
    void                            addListenChange(const string& _path_to_watch, const std::function<void (string, FileStatus)> &action);
};

#endif
