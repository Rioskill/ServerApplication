#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <string>
#include <iostream>

const int PORT = 8080;
bool Running = true;

char *read_from (int fd) {
    int size;
    read(fd, &size, 4);

    char *buffer = new char[size + 1];

    read(fd, buffer, size);
    buffer[size] = '\0';
    
    return buffer;
}

void send_size (int fd, const std::string &message) {
    const char *ch_message = message.c_str();
    int size = strlen(ch_message);

    send(fd, &size, sizeof(int), 0);
    send(fd, ch_message, size, 0);

    printf("sent \"%s\"\n", ch_message);
}

void send_endl (int fd, const std::string &message) {
    char *ch_message = new char[message.size() + 4];

    memcpy(ch_message, message.c_str(), message.size());
    ch_message[message.size()] = '\r';
    ch_message[message.size()+1] = '\n';
    ch_message[message.size()+2] = '\r';
    ch_message[message.size()+3] = '\n';

    int size = message.size() + 1;

    send(fd, ch_message, size, 0);

    printf("sent \"%s\"\n", message.c_str());

    delete[] ch_message;
}
 
int main(int argc, char const* argv[])
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    int pid = fork();

    if (pid == 0) {
        while (Running) {
            char *buffer = read_from(sock);

            printf("recieved \"%s\"\n", buffer);

            delete[] buffer;
        }
    } else {

        while (Running) {
            std::string input;
            std::getline(std::cin, input);

            if (input == "exit") {
                Running = false;
                kill(pid, SIGKILL);
            }

            std::string message = "GET / HTTP";

            send_endl(sock, message);
            // send_size(sock, input);
        }
    }

    int status;
    waitpid(pid, &status, 1);


 
    // closing the connected socket
    close(client_fd);
    return 0;
}