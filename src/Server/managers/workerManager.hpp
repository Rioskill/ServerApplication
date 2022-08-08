#ifndef WORKERMANAGER_H
#define WORKERMANAGER_H

#include <map>
#include <csignal>
#include <unistd.h>
#include <wait.h>
#include <queue>

#include "idAssigner.hpp"
#include "worker.hpp"

class WorkerManager {
private:
    std::map<int, Worker*> workers;
    std::map<int, int> worker_pids;
    ReuseIDAssigner id_assigner;

    EventLoop *loop;

    std::string in_pipe_name (int id);
    std::string out_pipe_name (int id);

public:
    WorkerManager (EventLoop *loop);
    ~WorkerManager();
    
    Worker *createWorker();
    void removeWorker (int id);
    void removeWorker (Worker *worker);

    void startWorker (int id);
    void startWorker (Worker *worker);

    void stopWorker (int id);
    void stopWorker (Worker *worker);

    Worker *getWorker (int id);
};

#endif