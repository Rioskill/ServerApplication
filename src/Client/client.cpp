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
            const char *message = input.c_str();
            int size = strlen(message);

            send(sock, &size, sizeof(int), 0);
            send(sock, message, strlen(message), 0);

            printf("sent \"%s\"\n", message);
        }
    }

    int status;
    waitpid(pid, &status, 1);
 
    // closing the connected socket
    close(client_fd);
    return 0;
}