#ifndef ASYNCOUTPUTSTREAM
#define ASYNCOUTPUTSTREAM

#include <unistd.h>
#include "stream.hpp"

class AsyncOutputStream: public Stream {
public:
    AsyncOutputStream (int fd, EventLoop *loop);
    AsyncOutputStream (): Stream(-1, nullptr) {}

    void write (int bytes, void *message, std::function<void()> cb);
};

#endif