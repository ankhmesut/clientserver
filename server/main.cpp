#include <iostream>
#include <chrono>
#include <thread>
#include <limits>
#include <cstdlib>
#include <iterator>

#include "ber.h"
#include "tcpserver.h"
#include "gmpxx.h"

static constexpr const char * DATA_LENGTH = "67108864";

std::string getHostStr(const Connection& client) {
  uint32_t ip = client.getHost ();
  return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
         std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
         std::to_string( client.getPort ());
}

//
// For demo purposes returns random char :)
// May read from file or smth.
//
char getNextByte() noexcept
{
    return 'A' + rand() % 26;
}

std::string sendLargeData(const Connection &client, const mpz_class &length, std::function<char(void)> nextByte)
{
    int chunkSize = 1024;
    mpz_class sent = 0;
    int ret = 0;

    if (length <= chunkSize) {
        std::unique_ptr<char[]> data(new char[length.get_ui()]);
        for (size_t i = 0; i < length.get_ui(); ++i)
            data[i] = nextByte();
        return std::to_string(client.sendData(data.get(), length.get_ui()));
    }

    while (sent < length) {
        std::unique_ptr<char[]> data(new char[chunkSize]);

        if (sent + chunkSize <= length) {
            for (size_t i = 0; i < chunkSize; ++i)
                data[i] = nextByte();
            ret = client.sendData(data.get(), chunkSize, false);
        } else if (sent != length) {
            for (size_t i = 0; i < length - sent; ++i)
                data[i] = nextByte();
            mpz_class l = length - sent;
            ret = client.sendData(data.get(), l.get_ui(), false);
        }

        if (ret < 0)
            return "0";
        else if (ret == 0)
            return "-1";
        else
            sent += ret;
    }

    return sent.get_str();
}

int main(int argc, char *argv[])
{
    ::srand(::time(NULL));

    TcpServer server( 8080,
          [](Connection client) {
              std::cout << "Thread: " << std::this_thread::get_id() << std::endl;

              // Print client address
              std::cout << "Connected host: " << getHostStr(client) << std::endl;

              std::vector<uint8_t> len_ber;
              convert_len_to_ber(DATA_LENGTH, len_ber);

              // Send BER encoded length octets
              std::cout << "Sent BER length bytes: "
                        << client.sendData((char *)len_ber.data(), len_ber.size(), false)
                        << ": ";
              std::copy(len_ber.cbegin(), len_ber.cend(), std::ostream_iterator<int>(std::cout, " "));
              std::cout << std::endl;

              // Send data part
              mpz_class len;
              len = DATA_LENGTH;

              std::cout << "Sent data bytes: "
                        << sendLargeData(client, len, getNextByte)
                        << std::endl;

              client.close();
          }
      );

      if(server.start() == Status::Up) {
          std::cout << "Server is up!" << std::endl;
          server.joinLoop();
      } else {
          std::cout << "Server start error! Error code:" << int(server.getStatus()) << std::endl;
          return -1;
      }

    return 0;
}
