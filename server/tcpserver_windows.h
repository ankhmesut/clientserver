#ifndef TCPSERVERWINDOWS_H
#define TCPSERVERWINDOWS_H

#include "tcpserver_p.h"

#include <winsock2.h>

//
// Windows-specific TCP server class implementation
//
class TcpServerWindows : public TcpServerPrivate
{
public:
    TcpServerWindows();
    TcpServerWindows(const uint16_t port, ConnectionHandler handler);

    ~TcpServerWindows() override;

    Status start() override;
    void stop() override;

    void handlingLoop() override;

private:
    SOCKET socket = INVALID_SOCKET;
    WSAData wsa;


};

#endif // TCPSERVERWINDOWS_H
