#include "Burst.hh"
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
  memcpy(m_input, that.m_input, burst_input_len);
  return *this;
}

int8_t const *Burst::data() const { return (int8_t const *)m_input; }

uint32_t Burst::frame_number() const { return m_frame_number; }

std::ostream &Burst::print_header(std::ostream &out) const {
  out << m_frame_number << "," << (int)m_time_slot << "," << (int)m_sub_slot
      << "," << m_channel_info;
  return out;
}

std::ostream &Burst::print(std::ostream &out) const {
  print_header(out);
  std::cout << ",";
  for (int i(0); i < burst_input_len; ++i) {
    out << (int)m_input[i];
  }
  return out;
}

Burst::Burst(char const *const buf)
    : m_time_slot(atoi(buf + 6)), m_frame_number(atoi(buf + 24)),
      m_sub_slot(atoi(buf + 32)), m_channel_info(m_time_slot, m_frame_number) {
  for (int i(48); i < 196; ++i) {
    if (buf[i] == '0') {
      m_input[i - 48] = 0;
    } else {
      m_input[i - 48] = 1;
    }
  }
}

ChannelInfo const &Burst::channel_info() const { return m_channel_info; }

std::ostream &operator<<(std::ostream &out, Burst const &b) {
  return b.print(out);
}

bool Burst::is_dummy() const {
  return memcmp(m_input, bust_dummy, burst_input_len) == 0;
}
