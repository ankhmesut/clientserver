#ifndef TCPSERVERLINUX_H
#define TCPSERVERLINUX_H

#include "tcpserver_p.h"

class TcpServerLinux : public TcpServerPrivate
{
public:
    TcpServerLinux();
    TcpServerLinux(const uint16_t port, ConnectionHandler handler);

    ~TcpServerLinux() override;

    Status start() override;
    void stop() override;

    void handlingLoop() override;

private:
    int socket;
};

#endif // TCPSERVERLINUX_H
