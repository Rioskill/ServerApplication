#ifndef NETWORKSOCKET
#define NETWORKSOCKET

#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "utils.hpp"

class NetworkSocket {
private:
    int port;
    char *ip;
    int fd;
public:
    NetworkSocket (const char *ip, int port): port(port) {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        int rc = inet_aton(ip, &addr.sin_addr);

        ASSERT(rc == 1, "failed to parse IP address");

        addr.sin_port = htons(port);

        fd = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT(fd >= 0, "failed to create listening socket");

        int opt_value = 1;
        rc = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value));
        ASSERT(!rc, "failed to set SO_REUSEADDR option for listening socket");

        int flags = fcntl(fd, F_GETFL);
        flags |= O_NONBLOCK;
        rc = fcntl(fd, F_SETFL, flags);
        ASSERT(rc != -1, "failed to set O_NONBLOCK flag on listening socket");

        rc = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
        ASSERT(!rc, "listening socket bing failed");

        rc = listen(fd, 4096);
        ASSERT(!rc, "listen() failed");
    }

    ~NetworkSocket() {
        shutdown(fd, SHUT_RDWR);
    }

    int get_fd () {
        return fd;
    }

    int accept_connection() {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        int incoming_fd = ::accept(fd, (struct sockaddr*)&addr, &addr_len);


        if (incoming_fd != -1) {
            int flags = fcntl(incoming_fd, F_GETFL);
            flags |= O_NONBLOCK;
            int rc = fcntl(incoming_fd, F_SETFL, flags);
            ASSERT(rc != -1, "failed to set O_NONBLOCK flag on accepted connection");
        }
        return incoming_fd;
    }
};

#endif