#ifndef STREAM
#define STREAM

#include "eventLoop.hpp"
#include "file.hpp"

class Stream: virtual public File {
protected:
    enum class Status {Working, Done};

    Status status;
    EventLoop *loop;
public:

    Stream (int fd, EventLoop *loop): File(fd), loop(loop) {}

    Status get_status() {return status;}

    void set_loop (EventLoop *loop) {
        this->loop = loop;
    }
};

#endif