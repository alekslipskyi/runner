#include <chrono>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <thread>
#include <experimental/filesystem>
#include <ctime>
#include <cstring>
#include <iostream>

#include "index.h"

using namespace std;

FileWatcher::FileWatcher(string path_to_watch, string path_to_file, chrono::duration<int, milli> delay) {
    this->running_ = true;
    this->path_to_watch = path_to_watch;
    this->path_to_file = path_to_file;
    this->delay = delay;

    if (strlen(path_to_watch.c_str()) != 0) {
        for(auto &file : filesystem::recursive_directory_iterator(path_to_watch)) {
            auto current_file_last_write_time = filesystem::last_write_time(file);
            this->paths_[file.path().string()] = current_file_last_write_time;
        }
    }

    if (strlen(path_to_file.c_str()) != 0) {
        this->current_file_last_write_time = filesystem::last_write_time(path_to_file);
    }
}

void FileWatcher::listenFile(const std::function<void (std::string, FileStatus)> &action) {
    while (this->running_) {
        this_thread::sleep_for(delay);
        filesystem::file_time_type current_file_last_write_time = filesystem::last_write_time(this->path_to_file);

        if(this->current_file_last_write_time != current_file_last_write_time) {
            this->current_file_last_write_time = current_file_last_write_time;
            action("", FileStatus::modified);
        }
    }
}

void FileWatcher::listenPath(const std::function<void (std::string, FileStatus)> &action) {
    while (this->running_) {
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
    }
}

bool FileWatcher::contains(const std::string &key) {
    auto el = paths_.find(key);
    return el != paths_.end();
}

