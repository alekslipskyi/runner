#include <chrono>
#include <unordered_map>
#include <string>
#include <functional>
#include <thread>
#include <experimental/filesystem>
#include <cstring>
#include <iostream>

#include "index.h"

using namespace std;

FileWatcher::FileWatcher(chrono::duration<int, milli> delay) {
    running_ = true;
    this->delay = delay;
}

void FileWatcher::addListenFile(string _path_to_file, const std::function<void (string, FileStatus)> &action) {
    path_to_file = _path_to_file;
    if (strlen(path_to_file.c_str()) != 0) {
        this->current_file_last_write_time = filesystem::last_write_time(path_to_file);
    }

    events.emplace_back([&] () -> void {
        this_thread::sleep_for(delay);
        filesystem::file_time_type current_file_last_write_time = filesystem::last_write_time(this->path_to_file);

        if(this->current_file_last_write_time != current_file_last_write_time) {
            this->current_file_last_write_time = current_file_last_write_time;
            action(path_to_watch, FileStatus::modified);
        }
    });
}

void FileWatcher::addListenPath(string _path_to_watch, const std::function<void (std::string, FileStatus)> &action) {
    path_to_watch = _path_to_watch;
    if (strlen(path_to_watch.c_str()) != 0) {
        for(auto &file : filesystem::recursive_directory_iterator(path_to_watch)) {
            auto current_file_last_write_time = filesystem::last_write_time(file);
            this->paths_[file.path().string()] = current_file_last_write_time;
        }
    }

    events.emplace_back([&] () -> void {
        auto it = this->paths_.begin();
        this_thread::sleep_for(delay);

        while (it != this->paths_.end()) {
            if (!experimental::filesystem::exists(it->first)) {
                action(it->first, FileStatus::erased);
                it = this->paths_.erase(it);
            } else {
                it++;
            }
        }

        for(auto &file : filesystem::recursive_directory_iterator(this->path_to_watch)) {
            auto current_file_last_write_time = filesystem::last_write_time(file);

            if(!this->contains(file.path().string())) {
                this->paths_[file.path().string()] = current_file_last_write_time;
                action(file.path().string(), FileStatus::created);
            } else {
                if(this->paths_[file.path().string()] != current_file_last_write_time) {
                    this->paths_[file.path().string()] = current_file_last_write_time;
                    action(file.path().string(), FileStatus::modified);
                }
            }
        }
    });
}

void FileWatcher::addListenChange(const string& _path_to_watch, const std::function<void(string, FileStatus)> &action) {
    if (filesystem::is_directory(_path_to_watch)) {
        addListenPath(_path_to_watch, action);
    } else {
        addListenFile(_path_to_watch, action);
    }
}

void FileWatcher::startListen() {
    while (running_) {
        for (auto &action: events) {
            action();
        }
    }
}

bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}

