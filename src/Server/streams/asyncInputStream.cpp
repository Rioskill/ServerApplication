#include "asyncInputStream.hpp"

AsyncInputStream::AsyncInputStream (int fd, unsigned int buffer_size, EventLoop *loop): File(fd), Stream(fd, loop), buffer(buffer_size) {}
AsyncInputStream::AsyncInputStream (const AsyncInputStream &other): Stream(fd, loop), buffer(other.buffer) {}

void AsyncInputStream::return_result (unsigned int bytes, unsigned int skip, std::function<void(int, char*)> cb) {

    std::cout << "returning result\n";

    char subbuf[bytes];
    memcpy(subbuf, buffer.data + buffer.start, bytes);
    buffer.start += bytes + skip;

    if (buffer.end < buffer.start)
        buffer.end = buffer.start;

    cb(bytes, subbuf);
}

void AsyncInputStream::read_possible (std::function<void()> cb) {
    ssize_t bytes_read = buffer.read_possible(get_fd());

    if (bytes_read == -1 || bytes_read == 0) {
        loop->schedule_on_readable(fd, [this, cb](){
            read_possible(cb);
        });
    } else {
        cb();
    }
}
 
void AsyncInputStream::read (unsigned int bytes, std::function<void(int, char*)> cb) {
    while (bytes > buffer.size)
        buffer.expand();

    if (bytes > buffer.size - buffer.start)
        buffer.release_used();

    if (buffer.end - buffer.start >= bytes) {
        return_result(bytes, 0, cb);
    } else {
        read_possible([this, bytes, cb](){
            read(bytes, cb);
        });
    }
}

void AsyncInputStream::read_until (const std::string &delimiter, std::function<void(int, char*)> cb) {

    // std::cout << "read_until exec\n";

    char *delimiter_pos = static_cast<char*>(memmem(buffer.data + buffer.start, buffer.end - buffer.start, delimiter.c_str(), delimiter.size()));

    if (delimiter_pos != nullptr) {
        return_result(delimiter_pos - (buffer.data + buffer.start), delimiter.size(), cb);
    } else {
        if (buffer.end == buffer.size - 1)
            buffer.expand();
        read_possible([this, delimiter, cb](){
            read_until(delimiter, cb);
        });
    }
}
