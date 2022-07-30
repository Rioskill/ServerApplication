#include "asyncOutputStream.hpp"

AsyncOutputStream::AsyncOutputStream (int fd, EventLoop *loop): File(fd), Stream(fd, loop) {}

void AsyncOutputStream::write (int bytes, void *message, std::function<void()> cb) {
    int written_bytes = ::write(fd, message, bytes);
    if (bytes == written_bytes) {
        cb();
    }
    else {
        loop->schedule_on_writeable(fd, [this, message, bytes, cb, written_bytes](){
            write(bytes - written_bytes, message + written_bytes, cb);
        });
    }
}
