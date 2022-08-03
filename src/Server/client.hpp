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
    Client (const File &fd, EventLoop *loop, int managerID=-1): in(fd.get_fd(), 20, loop), out(fd.get_fd(), loop), managerID(managerID) {}

    void read (std::function<void(int, char*)> cb) {
        in.read(4, [this, cb](char *data) {
            int size = static_cast<int>(*data);

            std::cout << "recieved size: " << size << std::endl;

            in.read(size, [size, cb](char *data) {
                cb(size, data);
            });
        });
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
};

#endif