#include <gmpxx.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <bitset>

#include <cstdlib>

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
//    len = a.get_str().c_str();

    return;
}

void convert_len_to_ber(const char *len, std::vector<uint8_t> &len_ber) noexcept
{
    mpz_class a, b, s;
    a = len;
    b = len;
    uint8_t cnt = 0;

    //
    // compute first byte of BER length field
    //
    if (a < 128) {
        cnt = a.get_ui(); // convert to uint
        len_ber.push_back(cnt);
        return;
    }
    else {
        // amount of length bytes in base 256
        cnt = 0b10000000;
        while (a > 256) {
            a /= 256;
            ++cnt;
        }
        ++cnt;
    }

//    std::cout << "cnt = " << std::bitset<8>(cnt) << " ";

    //
    // compute base 256 part of length bytes
    //
    uint8_t val;

    while ( b > 256) {
        s = b % 256;
        val = s.get_ui();
        len_ber.push_back(val);
        b /= 256;
    }

     val = b.get_ui();
     len_ber.push_back(val);

     len_ber.push_back(cnt);

     std::reverse(len_ber.begin(), len_ber.end());

//     std::copy(len_ber.cbegin(), len_ber.cend(), std::ostream_iterator<int>(std::cout, " "));
}
