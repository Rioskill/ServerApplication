#include "namedPipe.hpp"

NamedPipe::NamedPipe (const std::string &name): name(name), File(-1), opened(false) {}

NamedPipe::~NamedPipe () {
    close();
}

void NamedPipe::open (int mode, bool non_block) {
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

void NamedPipe::createFIFO() {
    int p = mkfifo(name.c_str(), 0666);
    if (p == -1)
        throw FIFOCreationException();
}

void NamedPipe::removeFIFO() {
    int p = unlink(name.c_str());
    if (p == -1)
        throw FIFORemovalException();
}