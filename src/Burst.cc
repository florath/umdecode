#include "Burst.hh"
#include "airprobe/a5_1_2.h"
#include <cstring>

uint8_t const bust_dummy[] = {
    0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0,
    1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0,
    0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0,
    0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1,
    1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0};

Burst const &Burst::operator=(Burst const &that) {
  m_time_slot = that.m_time_slot;
  m_frame_number = that.m_frame_number;
  m_sub_slot = that.m_sub_slot;
  m_channel_info = that.m_channel_info;
  memcpy(m_data, that.m_data, burst_len);
  return *this;
}

int8_t const *Burst::data() const { return (int8_t const *)m_data; }

uint32_t Burst::frame_number() const { return m_frame_number; }

std::ostream &Burst::print_header(std::ostream &out) const {
  out << m_frame_number << "," << (int)m_time_slot << "," << (int)m_sub_slot
      << "," << m_channel_info;
  return out;
}

std::ostream &Burst::print(std::ostream &out) const {
  print_header(out);
  std::cout << ",";
  for (int i(0); i < burst_len; ++i) {
    out << (int)m_data[i];
  }
  return out;
}

Burst::Burst(char const *const buf)
    : m_time_slot(atoi(buf + 6)), m_frame_number(atoi(buf + 24)),
      m_sub_slot(atoi(buf + 32)), m_channel_info(m_time_slot, m_frame_number) {
  for (int i(48); i < 196; ++i) {
    if (buf[i] == '0') {
      m_data[i - 48] = 0;
    } else {
      m_data[i - 48] = 1;
    }
  }
}

ChannelInfo const &Burst::channel_info() const { return m_channel_info; }

std::ostream &operator<<(std::ostream &out, Burst const &b) {
  return b.print(out);
}

bool Burst::is_dummy() const {
  return memcmp(m_data, bust_dummy, burst_len) == 0;
}

uint8_t Burst::time_slot() const { return m_time_slot; }

void Burst::run_a51(uint8_t *key) {
  uint8_t decrypted_data[burst_len];
  uint8_t AtoBkeystream[114];
  uint8_t BtoAkeystream[114];
  uint8_t *keystream;
  uint8_t *burst_binary(m_data);

  bool const uplink_burst(false);

  uint32_t const t1 = m_frame_number / (26 * 51);
  uint32_t const t2 = m_frame_number % 26;
  uint32_t const t3 = m_frame_number % 51;
  uint32_t const frame_number_mod = (t1 << 11) + (t3 << 5) + t2;

  std::cout << "BURST::decrypt ";
  for (int i(0); i < 8; ++i) {
    printf("%02x ", key[i]);
  }
  std::cout << std::endl;
  std::cout << "FrameNumberMod " << frame_number_mod << std::endl;

  keysetup(key, frame_number_mod);
  runA51(AtoBkeystream, BtoAkeystream);

  if (uplink_burst) {
    // process uplink burst
    keystream = BtoAkeystream;
  } else {
    // process downlink burst
    keystream = AtoBkeystream;
  }

  std::cout << "Keystream: ";
  for (int i(0); i < 114; ++i) {
    std::cout << (int)keystream[i];
  }
  std::cout << std::endl;

  /* guard bits */
  for (int i = 0; i < 3; i++) {
    decrypted_data[i] = burst_binary[i];
  }
  // encrypt first part of the burst
  for (int i = 0; i < 57; i++) {
    decrypted_data[i + 3] = keystream[i] ^ burst_binary[i + 3];
  }
  /* stealing bits and midamble */
  for (int i = 60; i < 88; i++) {
    decrypted_data[i] = burst_binary[i];
  }
  // encrypt second part of the burst
  for (int i = 0; i < 57; i++) {
    decrypted_data[i + 88] = keystream[i + 57] ^ burst_binary[i + 88];
  }
  /* guard bits */
  for (int i = 145; i < 148; i++) {
    decrypted_data[i] = burst_binary[i];
  }

  memcpy(m_data, decrypted_data, burst_len);
}
