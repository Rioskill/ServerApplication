#include "worker.hpp"

Worker::Worker (const std::string &in_pipe_name, const std::string &out_pipe_name, EventLoop *loop): in_pipe(in_pipe_name, loop), out_pipe(out_pipe_name, loop) {
    in_pipe.createFIFO();
    out_pipe.createFIFO();

    in_pipe.open(O_RDWR, true);
    out_pipe.open(O_RDWR, true);
}

Worker::~Worker () {}

void Worker::closeFIFOs() {
    in_pipe.close();
    out_pipe.close();

    in_pipe.removeFIFO();
    out_pipe.removeFIFO();
}

void Worker::read (std::function<void(int, char*)> cb) {
    in_pipe.read(4, [this, cb](char *data){
        int size = static_cast<int>(*data);

        std::cout << "recieved size: " << size << std::endl;

        in_pipe.read(size, [this, size, cb](char *data){
            std::string res(data, size);
            std::cout << "recieved: \"" << res << "\"\n";

            cb(size, data);
        });
    });
}

void Worker::write (int bytes, char *data, std::function<void()> cb) {
    out_pipe.write(sizeof(int), &bytes, [this, bytes, &data, cb](){
        out_pipe.write(bytes, data, [data, bytes, cb](){
            std::string str(data, bytes);

            std::cout << "sent \"" << str << "\"\n";

            cb();
        });
    });
}