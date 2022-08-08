#ifndef ASYNCOUTPUTSTREAM_H
#define ASYNCOUTPUTSTREAM_H

#include <unistd.h>
#include "stream.hpp"

class AsyncOutputStream: public Stream {
public:
    AsyncOutputStream (int fd, EventLoop *loop);
    AsyncOutputStream (): Stream(-1, nullptr) {}

    void write (unsigned int bytes, const void *message, std::function<void()> cb);
};

#endif