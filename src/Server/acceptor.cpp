#include "acceptor.hpp"

void Acceptor::accept() {
    int fd = socket.accept_connection();

    if (fd != -1) {
        std::cout << "accepted\n";
        AsyncInputStream *stream = new AsyncInputStream(fd, 20, loop);

        stream->read(sizeof(int), [loop=loop, fd, stream](char *data){
            int size = static_cast<int>(*data);

            stream->read(size, [fd, size, stream, loop](char *data){
                std::string res(data, size);
                std::cout << res << std::endl;

                AsyncOutputStream *out_stream = new AsyncOutputStream(fd, loop);
                out_stream->write(size, data, [out_stream]() {
                    delete out_stream;
                });

                delete stream;
            });
        });
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}

void WorkerEchoAcceptor::accept() {
    int fd = socket.accept_connection();

    if (fd != -1) {
        AsyncInputStream *stream = new AsyncInputStream(fd, 20, loop);

        stream->read(sizeof(int), [this, fd, stream](char *data){
            int size = static_cast<int>(*data);

            stream->read(size, [this, fd, size, stream](char *data){
                std::string res(data, size);
                std::cout << "recieved \"" << res << "\" from client"<< std::endl;

                worker->write(size, data, [this, fd]() {
                    worker->read([this, fd](int response_size, char *data){
                        AsyncOutputStream *out_stream = new AsyncOutputStream(fd, loop);
                        out_stream->write(response_size, data, [this, out_stream]() {
                            delete out_stream;
                        });
                    });
                });

                delete stream;
            });
        });
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}