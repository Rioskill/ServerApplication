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

    if (argc < 4) {
        std::cout << "Not enough arguments\n";
        return 1;
    }

    char *workerName = get_argument(argv, 1);
    char *inPipeName = get_argument(argv, 2);
    char *outPipeName = get_argument(argv, 3);

    printf("<%s> started\n", workerName);

    printf("<%s> pipe_names: %s %s\n", workerName, inPipeName, outPipeName);

    int in_fd = open(inPipeName, O_RDONLY);
    int out_fd = open(outPipeName, O_WRONLY);

    if (in_fd == -1) {
        printf("<%s> in_fd = -1\n", workerName);
        exit(1);
    }

    if (out_fd == -1) {
        printf("<%s> out_fd = -1\n", workerName);
        exit(1);
    }

    delete[] inPipeName;
    delete[] outPipeName;

    // while (true) {
    //     printf("<%s>\n", workerName);
    //     sleep(1);
    // }

    while (true) {
        int size;
        read(in_fd, &size, sizeof(int));

        printf("<%s> recieved size: %d\n", workerName, size);

        char *buffer = new char[size + 1];
        buffer[size] = '\0';
        read(in_fd, buffer, size);

        printf("<%s> recieved: \"%s\"\n", workerName, buffer);

        write(out_fd, &size, sizeof(int));

        write(out_fd, buffer, size);

        printf("<%s> sent: \"%s\"\n", workerName, buffer);

        delete[] buffer;
    }

    return 0;
}
