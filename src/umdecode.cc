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

uint32_t constexpr input_line_len(197);

void read_next_line(char *buf) {
  ssize_t const rres(::read(0, buf, 197));
  if (rres == 0) {
    // EOF
    exit(1);
  }

  if (rres != 197) {
    perror("Read (invalid byte cnt)");
    exit(1);
  }
}

int main() {
  // ToDo: SCH
  CCCHDecoder bcch_decoder;
  CCCHDecoder ccch_decoder[8];
  CCCHDecoder sdcch_decoder[8][8];
  CCCHDecoder sacch_decoder[8][8];

  char buf[input_line_len];

  while (true) {
    read_next_line(buf);

    Burst const b(buf);
    std::cout << b << std::endl;

    if (b.is_dummy()) {
      //      std::cout << "Skipping dummy Burst" << std::endl;
      continue;
    }

    uint8_t const time_slot(b.time_slot());
    uint8_t const sub_slot(b.channel_info().sub_slot());

    switch (b.channel_info().channel_type()) {
    case CCT::BCCH:
      bcch_decoder.decode(b);
      break;
    case CCT::CCCH:
      std::cout << "CCCH [" << (int)time_slot << "," << (int)sub_slot << "]"
                << std::endl;
      ccch_decoder[sub_slot].decode(b);
      break;
    case CCT::SDCCH:
      std::cout << "SDCCH [" << (int)time_slot << "," << (int)sub_slot << "]"
                << std::endl;
      sdcch_decoder[time_slot][sub_slot].decode(b);
      break;
    case CCT::SACCH:
      std::cout << "SACCH [" << (int)time_slot << "," << (int)sub_slot << "]"
                << std::endl;
      sacch_decoder[time_slot][sub_slot].decode(b);
      break;

    // Skip FCCH
    case CCT::FCCH:
      break;
    default:
      std::cout << b << std::endl;
    }
  }
}
