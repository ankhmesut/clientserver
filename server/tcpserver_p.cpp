#include "tcpserver_p.h"

TcpServerPrivate::TcpServerPrivate()
{

}

TcpServerPrivate::~TcpServerPrivate()
{

}

Status TcpServerPrivate::getStatus() const
{
    return status;
}

uint16_t TcpServerPrivate::getPort() const
{
    return port;
}

void TcpServerPrivate::setPort(const uint16_t port)
{
    this->port = port;
}

void TcpServerPrivate::setConnectionHandler(ConnectionHandler handler)
{
    this->handler = handler;
}

void TcpServerPrivate::joinLoop()
{
    loop.join();
}
