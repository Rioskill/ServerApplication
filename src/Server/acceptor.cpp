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

        int worker_id = workerManager->createWorker();
        workerManager->startWorker(worker_id);

        stream->read(sizeof(int), [this, fd, stream, worker_id](char *data){
            int size = static_cast<int>(*data);

            stream->read(size, [this, fd, size, stream, worker_id](char *data){
                std::string res(data, size);
                std::cout << "recieved \"" << res << "\" from client"<< std::endl;

                Worker *worker = workerManager->getWorker(worker_id);

                worker->write(size, data, [this, fd, worker_id, worker]() {
                    worker->read([this, fd, worker_id](int response_size, char *data){
                        AsyncOutputStream *out_stream = new AsyncOutputStream(fd, loop);
                        out_stream->write(response_size, data, [this, out_stream, worker_id]() {
                            delete out_stream;

                            workerManager->stopWorker(worker_id);
                            workerManager->removeWorker(worker_id);
                        });
                    });
                });

                delete stream;
            });
        });
    }
    loop->schedule_on_readable(socket.get_fd(), [this](){accept();});
}