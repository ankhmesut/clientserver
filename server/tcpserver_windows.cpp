#include "tcpserver_windows.h"

#include "connection.h"

TcpServerWindows::TcpServerWindows()
{

}

TcpServerWindows::TcpServerWindows(const uint16_t aport, ConnectionHandler ahandler)
{
    this->port = aport;
    this->handler=ahandler;
}

TcpServerWindows::~TcpServerWindows()
{
    if(status == Status::Up)
        stop();

    WSACleanup();
}

Status TcpServerWindows::start()
{
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKADDR_IN address;
    address.sin_addr.S_un.S_addr = INADDR_ANY;
    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    if((socket = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        return status = Status::ErrSocketInit;

    if(::bind(socket, (struct sockaddr *) &address, sizeof(address)) == SOCKET_ERROR)
        return status = Status::ErrSocketBind;

    if(::listen(socket, SOMAXCONN) == SOCKET_ERROR)
        return status = Status::ErrSocketListening;

    status = Status::Up;
    stopped = false;

    loop = std::thread([this]{ handlingLoop(); });

    return status;
}

void TcpServerWindows::stop()
{
    status = Status::Closed;
    stopped = true;
    closesocket(socket);
    joinLoop();
}

void TcpServerWindows::handlingLoop()
{
    while(!stopped) {
        SOCKET client_socket; //Сокет клиента
        SOCKADDR_IN client_addr; //Адресс клиента
        int addrlen = sizeof(client_addr); //Размер адреса клиента

        if ((client_socket = accept(socket, (struct sockaddr *) &client_addr, &addrlen)) != 0
                && status == Status::Up) {
                pool.enqueue(handler, Connection(client_socket, client_addr));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
