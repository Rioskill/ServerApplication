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

        int worker_id = workerManager->createWorker();
        workerManager->startWorker(worker_id);

        process_message(client, worker_id);
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}

void WorkerEchoAcceptor::close_connection (Client *client, int worker_id) {
    delete client;
    workerManager->stopWorker(worker_id);
    workerManager->removeWorker(worker_id);
}

void WorkerEchoAcceptor::process_message (Client *client, int worker_id) {
    client->read(sizeof(int), [this, client, worker_id](char *data){
        int size = static_cast<int>(*data);

        client->read(size, [this, client, size, worker_id](char *data){
            std::string res(data, size);
            std::cout << "recieved \"" << res << "\" from client"<< std::endl;

            if (res == "exit") {
                close_connection(client, worker_id);
            } else {
                Worker *worker = workerManager->getWorker(worker_id);
                worker->write(size, data, [this, worker_id, worker, client]() {
                    worker->read([this, worker_id, client](int response_size, char *data){
                        client->write(response_size, data, [this, client, worker_id]() {

                            process_message(client, worker_id);
                        });
                    });
                });
            }
        });
    });
}