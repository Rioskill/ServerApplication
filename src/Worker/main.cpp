#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string>
#include <iostream>
#include <cstring>

char *get_argument (char **argv, int n) {
    int arg_len = strlen(argv[n]) + 1;
    char *dest = new char[arg_len];

    strcpy(dest, argv[n]);
    dest[arg_len - 1] = '\0';

    return dest;
}

int main (int argc, char **argv) {

    if (argc < 2) {
        std::cout << "No arguments\n";
        return 1;
    }

    if (argc < 3) {
        std::cout << "Not enough arguments\n";
        return 1;
    }

    char *inPipeName = get_argument(argv, 1);
    char *outPipeName = get_argument(argv, 2);

    printf("%s %s\n", inPipeName, outPipeName);

    int in_fd = open(inPipeName, O_RDONLY);
    int out_fd = open(outPipeName, O_WRONLY);

    if (in_fd == -1) {
        std::cout << "in_fd = -1\n";
        exit(1);
    }

    if (out_fd == -1) {
        std::cout << "out_fd = -1\n";
        exit(1);
    }

    delete[] inPipeName;
    delete[] outPipeName;

    // int in_fd = open("toWorker", O_RDONLY);
    // int out_fd = open("fromWorker", O_WRONLY);

    while (true) {
        int size;
        read(in_fd, &size, sizeof(int));

        printf("<WORKER> recieved size: %d\n", size);

        char *buffer = new char[size + 1];
        buffer[size] = 0;
        read(in_fd, buffer, size);

        printf("<WORKER> recieved: \"%s\"\n", buffer);

        write(out_fd, &size, sizeof(int));

        write(out_fd, buffer, size);

        printf("<WORKER> sent: \"%s\"\n", buffer);

        delete[] buffer;
    }

    return 0;
}
