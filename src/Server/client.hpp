#ifndef CLIENT
#define CLIENT

#include <chrono>

#include "file.hpp"
#include "asyncInputStream.hpp"
#include "asyncOutputStream.hpp"

class Client {
protected:
    AsyncInputStream in;
    AsyncOutputStream out;

    int managerID;

public:
    Client (const File &file, EventLoop *loop, int managerID = -1): in(file.get_fd(), 20, loop), out(file.get_fd(), loop), managerID(managerID) {}

    void remove_schedules_from (EventLoop *loop) {
        loop->delete_schedules(in.get_fd());

        if (out.get_fd() != in.get_fd())
            loop->delete_schedules(out.get_fd());
    }

    void read_n_bytes (unsigned int n, std::function<void(int, char*)> cb) {
        in.read(n, cb);
    }

    void read (std::function<void(int, char*)> cb) {
        in.read_until("\r\n\r\n", cb);
    }

    void write (unsigned int bytes, const void *message, std::function<void()> cb){
        out.write(bytes, message, [this, cb](){
            char message[] = "\r\n\r\n";
            out.write(4, message, cb);
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

class HttpClient: public Client {
private:
    std::chrono::time_point<std::chrono::steady_clock> last_request_time;

    const std::chrono::duration<int> timeoutInterval = std::chrono::seconds(5);

public:
    HttpClient (const File &file, EventLoop *loop, int managerID = -1): Client(file, loop, managerID) {}

    void updateLastRequestTime() {
        last_request_time = std::chrono::steady_clock::now();
    }

    bool timeout() {
        auto now = std::chrono::steady_clock::now();

        return now >= last_request_time + timeoutInterval;
    }
};

#endif