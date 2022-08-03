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

Worker *WorkerManager::createWorker() {
    int id = id_assigner.assignId();

    workers.emplace(id, new Worker(in_pipe_name(id), out_pipe_name(id), loop, id));

    return workers[id];
}

void WorkerManager::removeWorker (int id) {
    workers[id]->closeFIFOs();
    delete workers[id];
    workers.erase(id);

    id_assigner.releaseId(id);
}

void WorkerManager::removeWorker (Worker *worker) {
    removeWorker(worker->getManagerID());
}

void WorkerManager::startWorker (int id) {
    int pid = fork();
    if (pid == 0) {
        std::string workerName = std::string("worker_") + std::to_string(id);
        int p = execl("worker", "worker", workerName.c_str(), out_pipe_name(id).c_str(), in_pipe_name(id).c_str(), NULL);
        std::cout << "failed to exec() worker" << std::endl;
    } else {
        worker_pids[id] = pid;
    }
}

void WorkerManager::startWorker (Worker *worker) {
    startWorker(worker->getManagerID());
}

void WorkerManager::stopWorker (int id) {
    kill(worker_pids[id], SIGKILL);

    int status;
    waitpid(worker_pids[id], &status, 1);
    worker_pids.erase(id);
}

void WorkerManager::stopWorker (Worker *worker) {
    stopWorker(worker->getManagerID());
}

Worker *WorkerManager::getWorker (int id) {
    return workers[id];
}