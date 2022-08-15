#include "asyncOutputStream.hpp"

AsyncOutputStream::AsyncOutputStream (int fd, EventLoop *loop): File(fd), Stream(fd, loop) {
    is_open = true;
}

void AsyncOutputStream::write (unsigned int bytes, const void *message, std::function<void()> cb) {

    if (!is_open)
        return;

    int written_bytes = ::write(fd, message, bytes);
    if (bytes == written_bytes) {
        cb();
    }
    else {
        loop->schedule_on_writeable(fd, [this, message, bytes, cb, written_bytes](){
            write(bytes - written_bytes, static_cast<const char*>(message) + written_bytes, cb);
        });
    }
}
