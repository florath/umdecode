// g++ -Wall -Wextra -std=c++14 gecode.cc
//
/*
2,4,3,7,    0, -30,   0,1234347,  6,  0,  0,
0,1010001000000111101100101100010000000000011110101000000001001111011110001001011101111001000010101010101010001001010100001000101111101000001001010000
*/

#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include "CCCHDecoder.hh"

int main() {
  char buf[197];

  CCCHDecoder ccch_decoder;
  CCCHDecoder bcch_decoder;

  while (true) {
    ssize_t const rres(::read(0, buf, 197));
    if (rres == 0) {
      // EOF
      break;
    }

    if (rres != 197) {
      perror("Read (invalid byte cnt)");
      exit(1);
    }

    Burst const b(buf);

    if (b.is_dummy()) {
      //      std::cout << "Skipping dummy Burst" << std::endl;
      continue;
    }

    switch (b.channel_info().channel_type()) {
    case CCT::BCCH:
      bcch_decoder.decode(b);
      break;
    case CCT::CCCH:
      ccch_decoder.decode(b);
      break;

    // Skip FCCH
    case CCT::FCCH:
      break;
    default:
      std::cout << b << std::endl;
    }
  }
}
