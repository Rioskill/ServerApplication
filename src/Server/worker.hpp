#ifndef WORKER_H
#define WORKER_H

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

    int managerID;
public:
    Worker (const std::string &in_pipe_name, const std::string &out_pipe_name, EventLoop *loop, int managerID=-1);
    ~Worker();

    void setManagerID (int managerID);
    int getManagerID();

    void closeFIFOs();

    void read (std::function<void(int, char*)> cb);
    void write (int bytes, char *data, std::function<void()> cb);
};

#endif