#ifndef BER_H
#define BER_H

#include <vector>
#include <stdint.h>

#include <gmpxx.h>

void convert_len_to_ber(const char *len, std::vector<uint8_t> &len_ber) noexcept;
void convert_ber_to_len(std::vector<uint8_t> &len_ber, mpz_class &len) noexcept;

#endif // BER_H
