#ifdef __WIN32

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define SOCKET int

#endif

#include <cstdlib>
#include <cstdio>

#include <gmpxx.h>
#include <vector>
#include <stdint.h>
#include <iostream>

#include <iterator>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "8080"

void convert_ber_to_len(std::vector<uint8_t> &len_ber, mpz_class &len) noexcept
{
    mpz_class a, b;
    a = 0; b = 1;

    if (len_ber.size() == 0) {
        len = 0;
        return;
    }

    uint8_t first = len_ber.at(0);

    if ((first & 0b10000000) == 0) {
        char buf[3];
        sprintf(buf, "%d", first);
        len = buf;
        return;
    }

    first &= 0b01111111;

    int index = 1;

    while (index < first) {
        for (int _i = 0; _i < first - index; ++_i)
            b *= 256;
        b *= len_ber[index];
        a += b;
        ++index;
        b = 0;
    }
    a += len_ber[index];

    len = a;

    return;
}

void receive_data_length(SOCKET sock, mpz_class &data_length)
{
    // Look first BER byte
    char first;
    recv(sock, &first, 1, 0);

    if (first & 0b10000000 == 0) {
        data_length = first;
    } else {
        first &= 0b01111111;

        std::cout << "BER length octets count = " << (int) first << std::endl;
        char *len_ber_char = new char[first + 1];

        int ret = recv(sock, len_ber_char + 1, first, 0);
        printf("Received BER octets: %d\n", ret);

        len_ber_char[0] = first | 0b10000000;

        std::vector<uint8_t> len_ber(len_ber_char, len_ber_char + first + 1);
        convert_ber_to_len(len_ber, data_length);

        std::cout << "BER octets: ";
        std::copy(len_ber.cbegin() + 1, len_ber.cend(), std::ostream_iterator<int>(std::cout, " "));
        std::cout<<std::endl;
    }
}

#ifdef __WIN32

int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int ret;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    ret = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (ret != 0) {
        printf("WSAStartup failed with error: %d\n", ret);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    ret = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
    if ( ret != 0 ) {
        printf("getaddrinfo failed with error: %d\n", ret);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a sock for connecting to server
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == INVALID_SOCKET) {
            printf("sock failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        ret = connect( sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (ret == SOCKET_ERROR) {
            closesocket(sock);
            sock = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (sock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    FILE *file = fopen("data.txt", "w");

    mpz_class data_length = 0;
    mpz_class total_recv = 0;

    receive_data_length(sock, data_length);

    // Receive until the peer closes the connection
    do {
        if (total_recv + recvbuflen > data_length && total_recv != data_length) {
            mpz_class t = data_length - total_recv;
            recvbuflen = t.get_ui();
        }

        ret = recv(sock, recvbuf, recvbuflen, 0);
        if ( ret > 0 ) {
            fprintf(file, "%*s", recvbuflen, recvbuf);
            total_recv += ret;
            printf("Bytes received: %d Total: %s\n", ret, total_recv.get_str().c_str());

        }

        else if ( ret == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( ret > 0 && total_recv <= data_length );

    printf("Total received: %s\n\nPress enter...", total_recv.get_str().c_str());
    getchar();

    fflush(file);
    fclose(file);

    // cleanup
    closesocket(sock);
    WSACleanup();

    return 0;
}

#else

int main(int argc, char **argv)
{
    int sock = 0;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    char recvbuf[DEFAULT_BUFLEN];
    int ret;
    int recvbuflen = DEFAULT_BUFLEN;

    memset( &hints, 0, sizeof(hints) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    ret = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
    if ( ret != 0 ) {
        printf("getaddrinfo failed with error: %d\n", ret);
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a sock for connecting to server
        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sock == -1) {
            printf("socket creation failed\n");
            return 1;
        }

        // Connect to server.
        ret = connect( sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (ret == -1) {
            close(sock);
            sock = 0;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (sock == 0) {
        printf("Unable to connect to server\n");
        return 1;
    }

    FILE *file = fopen("data.txt", "w");

    mpz_class data_length = 0;
    mpz_class total_recv = 0;

    receive_data_length(sock, data_length);

    // Receive until the peer closes the connection
    do {
        if (total_recv + recvbuflen > data_length && total_recv != data_length) {
            mpz_class t = data_length - total_recv;
            recvbuflen = t.get_ui();
        }

        ret = recv(sock, recvbuf, recvbuflen, 0);
        if ( ret > 0 ) {
            fprintf(file, "%*s", recvbuflen, recvbuf);
            total_recv += ret;
            printf("Bytes received: %d Total: %s\n", ret, total_recv.get_str().c_str());
        }

        else if ( ret == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed\n");

    } while( ret > 0 && total_recv <= data_length );

    printf("Total received: %s\n\nPress enter...", total_recv.get_str().c_str());
    getchar();

    fflush(file);
    fclose(file);

    // cleanup
    close(sock);

    return 0;
}

#endif
