#ifndef BURST_HH
#define BURST_HH

#include "ChannelInfo.hh"
#include <memory>

uint8_t const burst_len = 148;

class Burst {
public:
  Burst(char const *const buf);
  std::ostream &print_header(std::ostream &out) const;
  std::ostream &print(std::ostream &out) const;
  bool is_dummy() const;
  ChannelInfo const &channel_info() const;
  Burst const &operator=(Burst const &);
  int8_t const *data() const;
  uint32_t frame_number() const;
  uint8_t time_slot() const;

  void run_a51(uint8_t *key);

private:
  uint8_t m_time_slot;
  uint32_t m_frame_number;
  uint8_t m_sub_slot;
  ChannelInfo m_channel_info;
  uint8_t m_data[burst_len];
};

using Burst_sp = std::shared_ptr<Burst>;

std::ostream &operator<<(std::ostream &out, Burst const &b);

#endif
