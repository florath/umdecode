#include "Burst.hh"
#include <cstring>

Burst const & Burst::operator=(Burst const & that) {
  m_time_slot = that.m_time_slot;
  m_frame_number = that.m_frame_number;
  m_sub_slot = that.m_sub_slot;
  memcpy(m_input, that.m_input, burst_input_len);
  return *this;
}

int8_t const * Burst::data() const {
  return (int8_t const *)m_input;
}
