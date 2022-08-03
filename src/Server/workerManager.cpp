#include "workerManager.hpp"

std::string WorkerManager::in_pipe_name (int id) {
    return std::string("in_pipe_") + std::to_string(id);
}

std::string WorkerManager::out_pipe_name (int id) {
    return std::string("out_pipe_") + std::to_string(id);
}

WorkerManager::WorkerManager (EventLoop *loop): loop(loop) {
    std::cout << "size: " << deleted_worker_ids.size() << std::endl;
}

WorkerManager::~WorkerManager() {
    for (auto it = workers.begin(); it != workers.end(); it++) {
        const int id = it->first;
        stopWorker(id);
        
        workers[id]->closeFIFOs();
        delete workers[id];
    }
}

int WorkerManager::createWorker() {

    std::cout << "creating worker\n";
    std::cout << "size: " << deleted_worker_ids.size() << std::endl;

    int id;

    if (deleted_worker_ids.empty()) {
        std::cout << "empty\n";
        id = workers.size();
    } else {
        std::cout << "not empty\n";
        std::cout << "size: " << deleted_worker_ids.size() << std::endl;
        id = deleted_worker_ids.front();
        deleted_worker_ids.pop();
    }

    std::cout << "id: " << id << std::endl;

    workers.emplace(id, new Worker(in_pipe_name(id), out_pipe_name(id), loop));

    return id;
}

void WorkerManager::removeWorker(int id) {

    std::cout << "removing worker\n";

    workers[id]->closeFIFOs();
    delete workers[id];
    workers.erase(id);

    deleted_worker_ids.push(id);
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

void WorkerManager::stopWorker (int id) {
    kill(worker_pids[id], SIGKILL);

    int status;

    std::cout << "killing worker process\n";

    if (WIFEXITED(status))
        std::cout << "worker was killed succesfully\n";

    waitpid(worker_pids[id], &status, 1);
    worker_pids.erase(id);
}

Worker *WorkerManager::getWorker (int id) {
    return workers[id];
}