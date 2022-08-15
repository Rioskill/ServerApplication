#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <iostream>
#include <functional>
#include <queue>
#include <unordered_map>

class EventLoop {
private:

    enum class Status {Init, Working, Done};

    std::unordered_map<int, std::function<void()>> on_readable_callbacks;
    std::unordered_map<int, std::function<void()>> on_writeable_callbacks;

    std::queue<std::function<void()>> scheduled_callbacks;
    Status status;

    void empty_queue (std::queue<std::function<void()>> &queue);

public:

    EventLoop (): status(Status::Init){}

    void schedule (std::function<void()> callback);

    void delete_schedules (int fd);

    void schedule_on_readable (int fd, std::function<void()> callback);

    void schedule_on_writeable (int fd, std::function<void()> callback);

    void schedule_pack (std::unordered_map<int, std::function<void()>> &callbacks, fd_set &set);

    void set_fds_pack (std::unordered_map<int, std::function<void()>> &callbacks, fd_set &set, int &max_fd);

    void run();
    void stop();
};

#endif