#ifndef CONNECTION_H
#define CONNECTION_H

#ifdef __WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#include <stdint.h>
#include <functional>
#include <iostream>

enum class Status : uint8_t
{
    Up = 0,
    ErrSocketInit = 1,
    ErrSocketBind = 2,
    ErrSocketListening = 3,
    Closed = 4
};

static constexpr uint16_t buffer_size = 4096;

class Connection
{
public:
#ifdef _WIN32
    Connection(SOCKET socket, SOCKADDR_IN address)
        : socket(socket), address(address)
    {

    }
#else
    Connection(int socket, struct sockaddr_in address)
        : socket(socket), address(address)
    { }
#endif

    Connection(const Connection& other)
        : socket(other.socket), address(other.address)
    {

    }

    Connection(Connection&& other)
        : socket(other.socket), address(other.address)
    {

    }

    int loadData(char *buf, int length)
    {
        return recv(socket, buf, length, 0);
    }

    int loadAllData(char *bytes, int size)
    {
        size_t bytes_received = 0;

        while (bytes_received < size) {
            int result = ::recv(socket, &bytes[bytes_received], size - bytes_received, 0);

            if (result <= 0) {
                return result;
            }
            bytes_received += result;
        }
    }

    int sendData(const char* buffer, const int sz, bool once = true) const
    {
        int size = sz;
        const char *ptr = buffer;

        while (size)
        {
            int sent = ::send(socket, ptr, size, 0);

            if (sent < 0)
                return 0;
            else if (sent == 0)
                return -1;
            else
            {
                size -= sent;
                ptr += sent;
            }
        }

        if (once)
#ifdef __WIN32
            shutdown(socket, SD_SEND);
#else
            shutdown(socket, SHUT_WR);
#endif

        return sz;
    }

#ifdef __WIN32
    uint32_t getHost() const
    {
        return address.sin_addr.S_un.S_addr;
    }
    uint16_t getPort() const
    {
        return address.sin_port;
    }
    void close()
    {
        shutdown(socket, 0);
        closesocket(socket);
    }
    ~Connection()
    {

    }
#else
    uint32_t getHost() const
    {
        return address.sin_addr.s_addr;
    }
    uint16_t getPort() const
    {
        return address.sin_port;
    }
    void close()
    {
        shutdown(socket, 0);
        ::close(socket);
    }
    ~Connection() {

    }
#endif

private:
#ifdef __WIN32
    SOCKET socket;
    SOCKADDR_IN address;
#else
    int socket;
    struct sockaddr_in address;
#endif
};

using ConnectionHandler = std::function<void(Connection)>;

#endif // CONNECTION_H
