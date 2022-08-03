#ifndef ASYNCINPUTSTREAM
#define ASYNCINPUTSTREAM

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>

#include "stream.hpp"
#include "eventLoop.hpp"

class AsyncInputStream: public Stream {
protected:

    struct Buffer {
        char *data;
        unsigned int size, start, end;

        Buffer (unsigned int size): size(size), start(0), end(0) {
            data = new char[size];
        }

        Buffer (const Buffer &other): size(other.size), start(other.start), end(other.end) {
            data = new char[size];
            memcpy(data, other.data, size);
        }

        ~Buffer() {
            delete[] data;
        }

        ssize_t read_from (int fd, int bytes) {
            int res = ::read(fd, data + end, bytes - (end - start));
            return res;

        }

        void expand() {
            unsigned int new_size = size * 2;
            char *new_data = new char[new_size];

            memcpy(new_data, data + start, size - start);
            size = new_size;
            end -= start;
            start = 0;

            delete[] data;
            data = new_data;
        }

        void release_used() {
            memmove(data, data + start, size - start);
            end -= start;
            start = 0;
        }
    };

    Buffer buffer;
    void _read (unsigned int bytes, std::function<void(char*)> cb);
    void return_result (unsigned int bytes, std::function<void(char*)> cb);

public:
    AsyncInputStream (int fd, unsigned int buffer_size, EventLoop *loop);
    AsyncInputStream (): Stream(-1, nullptr), buffer(4) {}

    AsyncInputStream (const AsyncInputStream &);

    ~AsyncInputStream() = default;

    const char *get_data() {return buffer.data;}

    void read (unsigned int bytes, std::function<void(char*)> cb);
};

#endif