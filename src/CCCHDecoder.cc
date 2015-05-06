#include "CCCHDecoder.hh"
#include <iostream>
#include "airprobe/cch.h"
#include <cstring>
#include <unistd.h>
#include "airprobe/gsmtap.h"

#define DATA_BYTES 23


/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>

void send(uint8_t const * const data, uint32_t frame_number)
{
  uint32_t constexpr clen(sizeof(gsmtap_hdr) + DATA_BYTES);

  gsmtap_hdr header;
  header.version = 2;
  header.hdr_len = 4;
  header.type = 1;
  header.timeslot = 7;
  header.arfcn = 0;
  header.signal_dbm = -20;
  header.snr_db = -29;
  header.frame_number = htonl(frame_number);
  header.sub_type = 6; // ???
  header.antenna_nr = 123;
  header.sub_slot = 3;
  header.res = 0;
  
  uint8_t cdata[clen];
  memset(cdata, 0, clen);
  memcpy(cdata, &header, sizeof(gsmtap_hdr));
  memcpy(cdata + sizeof(gsmtap_hdr), data, DATA_BYTES);
  
   struct sockaddr_in servaddr;

   int sockfd(socket(AF_INET,SOCK_DGRAM,0));

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
   servaddr.sin_port=htons(4729);

   sendto(sockfd, cdata, clen, 0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
}

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

// tst005
// Kc Selbst berechnet (von Michael)
uint8_t key[8] =
    //   dfbfe95b43e11679
  {0xdf, 0xbf, 0xe9, 0x5b, 0x43, 0xe1, 0x16, 0x79 };

#if 0
// Der Key aus dem Test von Karsten Nohl
uint8_t key[8] =
    //   0x1EF00BAB3BAC7002
    {0x1E, 0xF0, 0x0B, 0xAB, 0x3B, 0xAC, 0x70, 0x02};
#endif

#if 0
uint64_t keys[10] = {
    0x6FF5C1A3411C2DD6, 0x411C2DD66FF5C1A3, 0x2DD6411CC1A36FF5,
    0xC1A36FF52DD6411C, 0xD62D1C41A3C1F56F, 0x35da1c4162c1d7fb,
    0x62c1d7fb35da1c41, 0xd7fb62c11c4135da, 0x1c4135dad7fb62c1,
    0xfbd7c162411cda35,
};

uint64_t keys[8] = {
    0x836E345EB6F22795UL, // Wie vom AT Kommando ausgegeben
    0xB6F22795836E345EUL, // Swap low / high 32 bit
    0x2795B6F2345E836EUL, // Swap low / high 32 bit and also 16bit
    0x9527F2B65E346E83UL, // Swap completely: bytewise

    0xA9E44F6D7A2C76C1UL, // Bitwise reverse
    0x7A2C76C1A9E44F6DUL, // Swap low / high 32 bit
    0x76C17A2C4F6DA9E4UL, // Swap low / high 32 bit and also 16 Bit
    0xC1762C7A6D4FE4A9UL, // Swap completely: bytewise
};
#endif

void CCCHDecoder::bursts4decrypt(uint8_t *key) {
#if 0
  uint8_t key[8] =
      //   0x1EF00BAB3BAC7002
      {0x1E, 0xF0, 0x0B, 0xAB, 0x3B, 0xAC, 0x70, 0x02};
#endif

  for (int i(0); i < 4; ++i) {
    std::cout << "Decryption" << std::endl;
    std::cout << m_bursts[i] << std::endl;
    m_bursts[i].run_a51(key);
    std::cout << m_bursts[i] << std::endl;
  }
}

bool CCCHDecoder::bursts4decode(uint8_t * outmsg) {
  static uint8_t min_error(255);

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

  if (errors < min_error) {
    min_error = errors;
    std::cout << "[MinErrorCnt " << errors << "]" << std::endl;
  }

  // check parity
  // If parity check error detected try to fix it.

  if (parity_check(decoded_data)) {
    std::cout << "Parity check" << std::endl;
    FC_init(&fc_ctx, 40, 184);
    unsigned char crc_result[PARITY_OUTPUT_SIZE];
    if (FC_check_crc(&fc_ctx, decoded_data, crc_result) == 0) {
      //("error: sacch: parity error (errors=%d fn=%d)\n", errors, ctx->fn);
      std::cout << "Uncorrectable errors!" << std::endl;
      errors = -1;
      return false;
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
  return true;
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

  uint8_t decrypted_data[DATA_BYTES];
  
  m_bursts[m_burst_cnt++] = b;
  if (m_burst_cnt == 4) {
    std::cout << "CCH decode [" << m_bursts[0].channel_info() << "]"
              << std::endl;

    //    for (uint16_t key_idx(0); key_idx < sizeof(keys); ++key_idx) {
    bursts4decrypt(key);
    bool const decode_ok = bursts4decode(decrypted_data);
    if (decode_ok == true) {
      std::cout << "Decode OK!" << std::endl;
      send(decrypted_data, m_bursts[0].frame_number());
      //      break;
      //      }
    }
    m_burst_cnt = 0;
  }
}
