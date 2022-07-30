#ifndef NAMEDPIPE
#define NAMEDPIPE

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <exception>

#include "asyncInputStream.hpp"
#include "asyncOutputStream.hpp"

class NamedPipe: virtual public File {
protected:
    std::string name;
    bool opened;

public:
    NamedPipe (const std::string &name);
    ~NamedPipe();

    void open (int mode, bool non_block);
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