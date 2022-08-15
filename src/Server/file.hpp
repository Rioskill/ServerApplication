#ifndef FILE_H
#define FILE_H

#include <exception>
#include <iostream>

class FileClosedException: public std::exception {
public:
    const char *what() const throw() override {
        return "Attempted to use closed file\n";
    }
};

class File {
protected:
    int fd;
    bool is_open;
public:
    File (int fd): fd(fd) {}

    File (): File(-1) {}

    int get_fd() const {
        if (fd != -1)
            return fd;
        throw FileClosedException();
    }
    void set_fd (int fd) {
        this->fd = fd;
    }

    void close() {
        if (is_open) {
            is_open = false;
            ::close(fd);
        }
    }
};

#endif