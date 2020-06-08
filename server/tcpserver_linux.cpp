#include "tcpserver_linux.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


TcpServerLinux::TcpServerLinux()
{

}

TcpServerLinux::TcpServerLinux(const uint16_t aport, ConnectionHandler ahandler)
{
    this->port = aport;
    this->handler=ahandler;
}

TcpServerLinux::~TcpServerLinux()
{
    if(status == Status::Up)
        stop();
}

Status TcpServerLinux::start()
{
    struct sockaddr_in server;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
    server.sin_family = AF_INET;

    socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if(socket == -1)
        return status = Status::ErrSocketInit;

    if(::bind(socket, (struct sockaddr *)&server, sizeof(server)) < 0)
        return status = Status::ErrSocketBind;

    if(::listen(socket, 3) < 0)
        return status = Status::ErrSocketListening;

    status = Status::Up;

    loop = std::thread([this]{ handlingLoop(); });

    return status;
}

void TcpServerLinux::stop()
{
    status = Status::Closed;
    stopped = true;
    close(socket);
    joinLoop();
}

void TcpServerLinux::handlingLoop()
{
    while(!stopped) {
        int client_socket; //Сокет клиента
        struct sockaddr_in client_addr; //Адресс клиента
        int addrlen = sizeof(client_addr); //Размер адреса клиента

        if ((client_socket = accept(socket, (struct sockaddr *) &client_addr, (socklen_t *) &addrlen)) != 0
                && status == Status::Up) {
                pool.enqueue(handler, Connection(client_socket, client_addr));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
