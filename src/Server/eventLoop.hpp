#ifndef EVENTLOOP
#define EVENTLOOP

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

public:

    EventLoop (): status(Status::Init){}

    void schedule(std::function<void()> callback);

    void schedule_on_readable (int fd, std::function<void()> callback);

    void schedule_on_writeable (int fd, std::function<void()> callback);

    void schedule_pack (std::unordered_map<int, std::function<void()>> &callbacks, fd_set &set);

    void set_fds_pack (std::unordered_map<int, std::function<void()>> &callbacks, fd_set &set, int &max_fd);

    void run();
};

#endif