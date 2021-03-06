#ifndef CCCH_DECODER_HH
#define CCCH_DECODER_HH

#include "Burst.hh"
#include "airprobe/cch.h"
#include "airprobe/fire_crc.h"

class CCCHDecoder {
public:
  CCCHDecoder();
  void decode(Burst const &b);

private:
  bool bursts4decode(uint8_t * data);
  void bursts4decrypt(uint8_t * key);

  Burst m_bursts[4];
  int m_burst_cnt;
  unsigned short interleave_trans[CONV_SIZE];
  FC_CTX fc_ctx;
};

#endif
