#ifndef BURST_HH
#define BURST_HH

#include "ChannelInfo.hh"

uint8_t const burst_input_len = 148;

class Burst {
public:
  Burst(char const *const buf);
  std::ostream & print_header(std::ostream & out) const;
  std::ostream &print(std::ostream &out) const;
  bool is_dummy() const;
  ChannelInfo const & channel_info() const;
  Burst const & operator=(Burst const &);
  int8_t const * data() const;

private:
  uint8_t m_time_slot;
  uint32_t m_frame_number;
  uint8_t m_sub_slot;
  ChannelInfo m_channel_info;
  uint8_t m_input[burst_input_len];
};

#endif
