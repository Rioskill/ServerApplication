#include "worker.hpp"

Worker::Worker (const std::string &in_pipe_name, const std::string &out_pipe_name, EventLoop *loop, int managerID)
: in_pipe(in_pipe_name, loop), out_pipe(out_pipe_name, loop), managerID(managerID) {
    in_pipe.createFIFO();
    out_pipe.createFIFO();

    in_pipe.open(O_RDWR, true);
    out_pipe.open(O_RDWR, true);
}

Worker::~Worker () {}

void Worker::setManagerID (int managerID) {
    this->managerID = managerID;
}

int Worker::getManagerID() {
    return managerID;
}

void Worker::closeFIFOs() {
    in_pipe.close();
    out_pipe.close();

    in_pipe.removeFIFO();
    out_pipe.removeFIFO();
}

void Worker::read (std::function<void(int, char*)> cb) {
    in_pipe.read(4, [this, cb](int message_size, char *data){
        int size = static_cast<int>(*data);
        in_pipe.read(size, [this, cb](int size, char *data){
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