#ifndef WORKER
#define WORKER

#include <unistd.h>
#include <fcntl.h>
#include <string>

#include "asyncInputStream.hpp"
#include "asyncOutputStream.hpp"
#include "namedPipe.hpp"

class Worker {
private:
    AsyncInputNamedPipeStream in_pipe;
    AsyncOutputNamedPipeStream out_pipe;
public:
    Worker (const std::string &in_pipe_name, const std::string &out_pipe_name, EventLoop *loop);
    ~Worker();

    void closeFIFOs();

    void read (std::function<void(int, char*)> cb);
    void write (int bytes, char *data, std::function<void()> cb);
};

#endif