#ifndef IDASSIGNER
#define IDASSIGNER

#include <queue>

class IdAssigner {
private:
    std::queue<int> deleted_ids;
    unsigned int counter;

public:
    IdAssigner(): counter(0) {}

    int countDeletedIds() {
        return deleted_ids.size();
    }

    int assignId () {
        if (deleted_ids.empty()) {
            return counter++;
        }

        int id = deleted_ids.front();
        deleted_ids.pop();
        return id;
    }

    void releaseId(int id) {
        deleted_ids.push(id);
    }
};

#endif