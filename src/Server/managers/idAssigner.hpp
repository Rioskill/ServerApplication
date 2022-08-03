#ifndef IDASSIGNER
#define IDASSIGNER

#include <queue>

class IDAssigner {
public:
    virtual int assignID() = 0;
};

class ReuseIDAssigner: public IDAssigner {
private:
    std::queue<int> deleted_ids;
    unsigned int counter;

public:
    ReuseIDAssigner(): counter(0) {}

    int countDeletedIDs() {
        return deleted_ids.size();
    }

    int assignID() override {
        if (deleted_ids.empty()) {
            return counter++;
        }

        int id = deleted_ids.front();
        deleted_ids.pop();
        return id;
    }

    void releaseID(int id) {
        deleted_ids.push(id);
    }
};

class UniqueIDAssigner: public IDAssigner {
private:
    unsigned int counter;

public:
    UniqueIDAssigner(): counter(0) {}

    int assignID() override {
        return counter++;
    }
};

#endif