#include "tcpserver.h"

#include <cstdint>
#include <functional>
#include <thread>
#include <list>

#if defined(_WIN32)

#include "tcpserver_windows.h"

#elif defined(__linux__)

#include "tcpserver_linux.h"

#else
#error Unsupported platform
#endif

TcpServer::TcpServer()
#ifdef __WIN32
    : d(new TcpServerWindows)
#else
    : d(new TcpServerLinux)
#endif
{

}

TcpServer::TcpServer(const uint16_t port, ConnectionHandler handler)
#ifdef __WIN32
    : d(new TcpServerWindows(port, handler))
#else
    : d(new TcpServerLinux(port, handler))
#endif
{

}

TcpServer::~TcpServer()
{

}

void TcpServer::setHandler(ConnectionHandler handler)
{
    d->setConnectionHandler(handler);
}

Status TcpServer::getStatus() const
{
    return d->getStatus();
}

uint16_t TcpServer::getPort() const
{
    return d->getPort();
}

void TcpServer::setPort(const uint16_t port)
{
    d->setPort(port);
}

Status TcpServer::start()
{
    return d->start();
}

void TcpServer::joinLoop()
{
    d->joinLoop();
}

