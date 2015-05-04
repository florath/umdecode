#include "CCCHDecoder.hh"
#include <iostream>
#include "airprobe/cch.h"
#include <cstring>

#define DATA_BYTES 23

char const *burst_init = "2,4,3,7,    0, -28,   0,     99,  6,  0,  0,  "
                         "0,"
                         "00000000000000000000000000000000000000000000000000000"
                         "00000000000000000000000000000000000000000000000000000"
                         "000000000000000000000000000000000000000000";

CCCHDecoder::CCCHDecoder()
    : m_bursts({burst_init, burst_init, burst_init, burst_init}),
      m_burst_cnt(0) {
  // initialize de/interleaver
  int j, k, B;
  for (k = 0; k < CONV_SIZE; k++) {
    B = k % 4;
    j = 2 * ((49 * k) % 57) + ((k % 8) / 4);
    interleave_trans[k] = B * 114 + j; // 114=57 + 57
  }

  // initialize decoder
  FC_init(&fc_ctx, 40, 184);
}

void CCCHDecoder::bursts4decode() {
  unsigned char iBLOCK[BLOCKS * iBLOCK_SIZE], conv_data[CONV_SIZE],
      decoded_data[PARITY_OUTPUT_SIZE];

  for (int ii = 0; ii < 4; ii++) {
    for (int jj = 0; jj < 57; jj++) {
      iBLOCK[ii * iBLOCK_SIZE + jj] = m_bursts[ii].data()[jj + 3];
      iBLOCK[ii * iBLOCK_SIZE + jj + 57] =
          m_bursts[ii].data()[jj + 88]; // 88 = 3+57+1+26+1
    }
  }
  // deinterleave
  for (int k = 0; k < CONV_SIZE; k++) {
    conv_data[k] = iBLOCK[interleave_trans[k]];
  }
  // convolutional code decode
  int errors = conv_decode(decoded_data, conv_data);
  std::cout << "Errors:" << errors << " " << parity_check(decoded_data)
            << std::endl;
  // check parity
  // If parity check error detected try to fix it.

  if (parity_check(decoded_data)) {
    FC_init(&fc_ctx, 40, 184);
    unsigned char crc_result[PARITY_OUTPUT_SIZE];
    if (FC_check_crc(&fc_ctx, decoded_data, crc_result) == 0) {
      //("error: sacch: parity error (errors=%d fn=%d)\n", errors, ctx->fn);
      std::cout << "Uncorrectable errors!" << std::endl;
      errors = -1;
      return;
    } else {
      // DEBUGF("Successfully corrected parity bits! (errors=%d fn=%d)\n",
      // errors, ctx->fn);
      std::cout << "Corrected some errors" << std::endl;
      memcpy(decoded_data, crc_result, PARITY_OUTPUT_SIZE);
      errors = 0;
    }
  } else {
    std::cout << "Everything correct" << std::endl;
  }
  // compress bits
  unsigned char outmsg[28];
  unsigned char sbuf_len = 224;
  int i, j, c, pos = 0;
  for (i = 0; i < sbuf_len; i += 8) {
    for (j = 0, c = 0; (j < 8) && (i + j < sbuf_len); j++) {
      c |= (!!decoded_data[i + j]) << j;
    }
    outmsg[pos++] = c & 0xff;
  }

  for (int u(0); u < 4; ++u) {
    std::cout << "[";
    m_bursts[u].print_header(std::cout);
    std::cout << "] ";
  }

  for (int u(0); u < DATA_BYTES; ++u) {
    printf("%02x ", outmsg[u]);
  }
  printf("\n");
}

void CCCHDecoder::decode(Burst const &b) {
  //  std::cout << "CCCH add data [" << m_burst_cnt << "]" << std::endl;

  if (m_burst_cnt > 0) {
    if (m_bursts[m_burst_cnt - 1].frame_number() + 1 != b.frame_number()) {
      std::cout << "BURSTS not incrementing" << std::endl;
      m_burst_cnt = 0;
      return;
    }
  }

  if (b.channel_info().burst_offset() != m_burst_cnt) {
    std::cout << "BURST count not in sync with array offset" << std::endl;
    m_burst_cnt = 0;
    return;
  }

  m_bursts[m_burst_cnt++] = b;
  if (m_burst_cnt == 4) {
    std::cout << "CCH decode [" << m_bursts[0].channel_info() << "]"
              << std::endl;
    bursts4decode();
    m_burst_cnt = 0;
  }
}
