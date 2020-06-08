#ifndef TCPSERVER_P_H
#define TCPSERVER_P_H

#include "connection.h"
#include "threadpool.h"

#include <thread>
#include <atomic>

//
// TcpServerPrivate base class
//
class TcpServerPrivate
{
public:
    TcpServerPrivate();

    virtual Status start() = 0;
    virtual void stop() = 0;

    virtual void handlingLoop() = 0;

    virtual ~TcpServerPrivate();

    Status getStatus() const;

    uint16_t getPort() const;
    void setPort(const uint16_t port);

    void setConnectionHandler(ConnectionHandler handler);
    void joinLoop();

protected:
    ConnectionHandler handler;
    std::thread loop;
    ThreadPool pool;
    Status status;
    uint16_t port;
    std::atomic_bool stopped;
};

#endif // TCPSERVER_P_H
