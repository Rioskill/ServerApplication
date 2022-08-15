#include "asyncInputStream.hpp"

AsyncInputStream::AsyncInputStream (int fd, unsigned int buffer_size, EventLoop *loop): File(fd), Stream(fd, loop), buffer(buffer_size) {
    is_open = true;
}
AsyncInputStream::AsyncInputStream (const AsyncInputStream &other): Stream(fd, loop), buffer(other.buffer) {
    is_open = true;   
}

void AsyncInputStream::return_result (unsigned int bytes, unsigned int skip, std::function<void(int, char*)> cb) {
    char subbuf[bytes];
    memcpy(subbuf, buffer.data + buffer.start, bytes);
    buffer.start += bytes + skip;

    if (buffer.end < buffer.start)
        buffer.end = buffer.start;

    cb(bytes, subbuf);
}

// returns true if socket is not closed
bool AsyncInputStream::read_possible (std::function<void()> cb) {

    if (!is_open)
        return false;

    ssize_t bytes_read = buffer.read_possible(get_fd());

    if (bytes_read == 0) {
        std::cout << "zero bytes read\n";
        return false; // EOF => socket is closed
    }

    if (bytes_read == -1) {
        loop->schedule_on_readable(fd, [this, cb](){
            read_possible(cb);
        }); 
    } else {
        cb();
    }

    return true;
}
 
void AsyncInputStream::read (unsigned int bytes, std::function<void(int, char*)> cb) {
    while (bytes > buffer.size)
        buffer.expand();

    if (bytes > buffer.size - buffer.start)
        buffer.release_used();

    if (buffer.end - buffer.start >= bytes) {
        return_result(bytes, 0, cb);
    } else {
        bool res = read_possible([this, bytes, cb](){
            read(bytes, cb);
        });

        if (res == false) {
            cb(0, nullptr);
        }
    }
}

void AsyncInputStream::read_until (const std::string &delimiter, std::function<void(int, char*)> cb) {
    char *delimiter_pos = static_cast<char*>(memmem(buffer.data + buffer.start, buffer.end - buffer.start, delimiter.c_str(), delimiter.size()));

    if (delimiter_pos != nullptr) {
        return_result(delimiter_pos - (buffer.data + buffer.start), delimiter.size(), cb);
    } else {
        if (buffer.end == buffer.size - 1)
            buffer.expand();
        bool res = read_possible([this, delimiter, cb](){
            read_until(delimiter, cb);
        });

        if (res == false) {
            cb(0, nullptr);
        }
    }
}
