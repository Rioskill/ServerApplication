#include "namedPipe.hpp"

NamedPipe::NamedPipe (const std::string &name): name(name), File(-1), opened(false) {}

NamedPipe::~NamedPipe () {
    close();
}

void NamedPipe::open (int mode = O_RDWR, bool non_block = true) {
    fd = ::open(name.c_str(), mode);

    if (non_block) {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    opened = true;
}

void NamedPipe::close() {
    if (opened) {
        ::close(fd);
        opened = false;
    }
}
