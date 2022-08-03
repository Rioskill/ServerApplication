#include "acceptor.hpp"

void Acceptor::accept() {
    int fd = socket.accept_connection();

    if (fd != -1) {
        std::cout << "accepted\n";
        processor->process(fd);
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}
