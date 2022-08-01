#include "workerManager.hpp"

std::string WorkerManager::in_pipe_name (int id) {
    return std::string("in_pipe_") + std::to_string(id);
}

std::string WorkerManager::out_pipe_name (int id) {
    return std::string("out_pipe_") + std::to_string(id);
}

WorkerManager::WorkerManager (EventLoop *loop): loop(loop) {}

WorkerManager::~WorkerManager() {
    for (auto it = workers.begin(); it != workers.end(); it++) {
        const int id = it->first;
        stopWorker(id);
        
        workers[id]->closeFIFOs();
        delete workers[id];
    }
}

int WorkerManager::createWorker() {
    int id = workers.size();

    workers.emplace(id, new Worker(in_pipe_name(id), out_pipe_name(id), loop));

    return id;
}

void WorkerManager::removeWorker(int id) {
    workers[id]->closeFIFOs();
    delete workers[id];
    workers.erase(id);
}

void WorkerManager::startWorker (int id) {
    int pid = fork();
    if (pid == 0) {
        int p = execl("worker", "worker", out_pipe_name(id).c_str(), in_pipe_name(id).c_str(), NULL);
        std::cout << "failed to exec() worker" << std::endl;
    } else {
        worker_pids[id] = pid;
    }
}

void WorkerManager::stopWorker (int id) {
    kill(worker_pids[id], SIGKILL);

    int status;

    waitpid(worker_pids[id], &status, 1);
    worker_pids.erase(id);
}