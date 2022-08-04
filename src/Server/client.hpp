#ifndef CLIENT
#define CLIENT

#include "file.hpp"
#include "asyncInputStream.hpp"
#include "asyncOutputStream.hpp"

class Client {
private:
    AsyncInputStream in;
    AsyncOutputStream out;

    int managerID;

public:
    Client (const File &file, EventLoop *loop, int managerID = -1): in(file.get_fd(), 20, loop), out(file.get_fd(), loop), managerID(managerID) {}

    void read (std::function<void(int, char*)> cb) {
        in.read_until("\n", cb);
    }

    void write (unsigned int bytes, void *message, std::function<void()> cb){
        out.write(sizeof(int), &bytes, [this, bytes, message, cb](){
            out.write(bytes, message, cb);
        });
    }

    void setManagerID (int managerID) {
        this->managerID = managerID;
    }

    int getManagerID() {
        return managerID;
    }

    void close() {
        ::close(in.get_fd());
    }
};

#endif