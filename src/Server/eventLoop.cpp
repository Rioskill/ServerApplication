#include "eventLoop.hpp"

void EventLoop::schedule(std::function<void()> callback) {
    scheduled_callbacks.push(callback);
}

void EventLoop::schedule_on_readable (int fd, std::function<void()> callback) {
    on_readable_callbacks.emplace(fd, callback);
}

void EventLoop::schedule_on_writeable (int fd, std::function<void()> callback) {
    on_writeable_callbacks.emplace(fd, callback);
}

void EventLoop::schedule_pack (std::unordered_map<int, std::function<void()>> &callbacks, fd_set &set) {
    for (auto it = callbacks.begin(); it != callbacks.end();) {
        auto fd = it->first;
        auto cb = it->second;

        if (FD_ISSET(fd, &set)) {
            schedule(cb);
            it = callbacks.erase(it);
        } else {
            ++it;
        }
    }
}

void EventLoop::set_fds_pack (std::unordered_map<int, std::function<void()>> &callbacks, fd_set &set, int &max_fd) {
    for (auto& pair: callbacks) {
        auto fd = pair.first;
        auto cb = pair.second;

        if (fd > max_fd)
            max_fd = fd;

        FD_SET(fd, &set);
    }
}

void EventLoop::stop() {
    status = Status::Done;
}

void EventLoop::run() {
    status = Status::Working;

    std::cout << "EventLoop started\n";

    while (status == Status::Working) {
        fd_set read_fds;
        fd_set write_fds;

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        int max_fd = -1;

        set_fds_pack(on_readable_callbacks, read_fds, max_fd);
        set_fds_pack(on_writeable_callbacks, write_fds, max_fd);

        while (select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) == -1)
            continue;
            
        schedule_pack(on_readable_callbacks, read_fds);
        schedule_pack(on_writeable_callbacks, write_fds);

        while (!scheduled_callbacks.empty()) {
            scheduled_callbacks.front()();
            scheduled_callbacks.pop();
        }
    }
}