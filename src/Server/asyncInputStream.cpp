#include "asyncInputStream.hpp"

AsyncInputStream::AsyncInputStream (int fd, unsigned int buffer_size, EventLoop *loop): File(fd), Stream(fd, loop), buffer(buffer_size) {}
AsyncInputStream::AsyncInputStream (const AsyncInputStream &other): Stream(fd, loop), buffer(other.buffer) {}

void AsyncInputStream::return_result (unsigned int bytes, std::function<void(char*)> cb) {
    char subbuf[bytes];
    memcpy(subbuf, buffer.data + buffer.start, bytes);
    buffer.start += bytes;
    cb(subbuf);
}

void AsyncInputStream::_read (unsigned int bytes, std::function<void(char*)> cb) {
    if (status == Status::Working) {
        ssize_t bytes_read = buffer.read_from(get_fd(), bytes);

        if (bytes_read == -1 || bytes_read == 0) {
            loop->schedule_on_readable(fd, [this, bytes, cb](){_read(bytes, cb);});
            return;
        }

        buffer.end += bytes_read;

        if (buffer.end - buffer.start >= bytes) {
            status = Status::Done;
            return_result(bytes, cb);
        } else
            loop->schedule_on_readable(fd, [this, bytes, cb](){_read(bytes, cb);});

    } else if (status == Status::Done) {
        return_result(bytes, cb);
    }
}

void AsyncInputStream::read (unsigned int bytes, std::function<void(char*)> cb) {

    // std::cout << "read from fd " << get_fd() << std::endl;

    if (buffer.end - buffer.start >= bytes) {
        return_result(bytes, cb);
        return;
    }

    status = Status::Working;

    while (bytes > buffer.size)
        buffer.expand();

    if (bytes > buffer.size - buffer.start)
        buffer.release_used();

    _read(bytes, cb);
}
