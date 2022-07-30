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
    Worker (const std::string &in_pipe_name,
            const std::string &out_pipe_name,
            EventLoop *loop): in_pipe(in_pipe_name, loop), out_pipe(out_pipe_name, loop) {

        in_pipe.open(O_RDWR, true);
        out_pipe.open(O_RDWR, true);
    }

    void read (std::function<void(int, char*)> cb) {
        in_pipe.read(sizeof(int), [this, cb](char *data){
            int size = static_cast<int>(*data);

            std::cout << "recieved size: " << size << std::endl;

            in_pipe.read(size, [this, size, cb](char *data){
                std::string res(data, size);
                std::cout << "recieved: " << res << std::endl;

                cb(size, data);
            });
        });
    }

    void write (int bytes, char *data, std::function<void()> cb) {
        out_pipe.write(sizeof(int), &bytes, [this, bytes, &data, cb](){
            out_pipe.write(bytes, data, [data, bytes, cb](){
                std::string str(data, bytes);

                std::cout << "sent " << str << std::endl;

                cb();
            });
        });
    }
};

#endif