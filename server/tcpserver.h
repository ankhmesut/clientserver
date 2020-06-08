#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "connection.h"

#include <memory>

class TcpServerPrivate;

class TcpServer
{
public:  
    TcpServer();
    TcpServer(const uint16_t port, ConnectionHandler handler);
    ~TcpServer();

    void setHandler(ConnectionHandler handler);

    Status getStatus() const;

    uint16_t getPort() const;
    void setPort(const uint16_t port);

    Status restart();
    Status start();
    void stop();

    void joinLoop();

protected:
    std::unique_ptr<TcpServerPrivate> d;
};

#endif // TCPSERVER_H
