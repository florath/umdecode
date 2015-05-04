#ifndef CHANNEL_INFO_HH
#define CHANNEL_INFO_HH

#include "ChannelType.hh"
#include <iostream>

class ChannelInfo {
public:
  ChannelInfo(uint8_t const time_slot, uint32_t const frame_number);
  std::ostream &print(std::ostream &out) const;

  CCT channel_type() const;
  uint8_t burst_offset() const;

private:
  CCT m_cct;
  uint8_t m_sub_slot;
  uint8_t m_burst_offset;
};

inline std::ostream &operator<<(std::ostream &out, ChannelInfo const &ci) {
  return ci.print(out);
}

#endif
