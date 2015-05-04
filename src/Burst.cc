#include "Burst.hh"
#include <cstring>

Burst const & Burst::operator=(Burst const & that) {
  m_time_slot = that.m_time_slot;
  m_frame_number = that.m_frame_number;
  m_sub_slot = that.m_sub_slot;
  m_channel_info = that.m_channel_info;
  memcpy(m_input, that.m_input, burst_input_len);
  return *this;
}

int8_t const * Burst::data() const {
  return (int8_t const *)m_input;
}

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
