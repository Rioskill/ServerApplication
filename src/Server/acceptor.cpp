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
        Client *client = new Client(fd, loop);

        int client_id = room->addClient(client);
        process_message(client, client_id);
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}

void WorkerEchoAcceptor::close_connection (Client *client, int client_id) {
    delete client;
    room->removeClient(client_id);
}

void WorkerEchoAcceptor::process_message (Client *client, int client_id) {
    client->read([this, client, client_id](int size, char *data){
        std::string res(data, size);
        std::cout << "recieved \"" << res << "\" from client"<< std::endl;

        if (res == "exit") {
            close_connection(client, client_id);
        } else {
            Worker *worker = room->getWorker();

            worker->write(size, data, [this, client_id, worker, client]() {
                worker->read([this, client_id, client](int response_size, char *data){
                    room->broadcast(response_size, data, [this, client, client_id]() {

                        process_message(client, client_id);
                    });
                });
            });
        }
    });
}