#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <iostream>
#include "eventLoop.hpp"
#include "networkSocket.hpp"
#include "client.hpp"

#include "room.hpp"
#include "connectionProcessor.hpp"
#include "workerManager.hpp"

class Acceptor {
protected:
    ConnectionProcessor *processor;
    EventLoop *loop;
    NetworkSocket &socket;

public:
    Acceptor (NetworkSocket &socket, EventLoop *loop, ConnectionProcessor *processor): socket(socket), loop(loop), processor(processor) {}

    void accept();
};

#endif