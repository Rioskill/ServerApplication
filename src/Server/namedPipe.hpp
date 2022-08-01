#ifndef NAMEDPIPE
#define NAMEDPIPE

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <exception>

#include "asyncInputStream.hpp"
#include "asyncOutputStream.hpp"

class FIFODCreationException: std::exception {
public:
    const char *what() const throw() {
        return "Cannot create FIFO\n";
    }
};

class FIFODRemovalException: std::exception {
public:
    const char *what() const throw() {
        return "Cannot remove FIFO\n";
    }
};

class NamedPipe: virtual public File {
protected:
    std::string name;
    bool opened;

public:
    NamedPipe (const std::string &name);
    ~NamedPipe();

    void createFIFO();
    void removeFIFO();

    void open (int mode = O_RDWR, bool non_block = true);
    void close();
};

class AsyncInputNamedPipeStream: public NamedPipe, public AsyncInputStream {
public:
    AsyncInputNamedPipeStream (const std::string &name, EventLoop *loop, unsigned int buffer_size=4):
    NamedPipe(name), AsyncInputStream(-1, buffer_size, loop) {}
};

class AsyncOutputNamedPipeStream: public NamedPipe, public AsyncOutputStream {
public:
    AsyncOutputNamedPipeStream (const std::string &name, EventLoop *loop):
    NamedPipe(name), AsyncOutputStream(-1, loop) {}
};

#endif