#include "acceptor.hpp"

void Acceptor::accept() {
    int fd = socket.accept_connection();

    if (fd != -1) {
        std::cout << "accepted\n";

        Client *client = new Client(fd, loop);

        processor->process(client);
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}
