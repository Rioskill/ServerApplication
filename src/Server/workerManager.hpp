#ifndef WORKERMANAGER
#define WORKERMANAGER

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
    IdAssigner id_assigner;

    EventLoop *loop;

    std::string in_pipe_name (int id);
    std::string out_pipe_name (int id);

public:
    WorkerManager (EventLoop *loop);
    ~WorkerManager();
    
    int createWorker();
    void removeWorker(int id);

    void startWorker (int id);
    void stopWorker (int id);

    Worker *getWorker (int id);
};

#endif