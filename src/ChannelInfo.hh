#ifndef CHANNEL_INFO_HH
#define CHANNEL_INFO_HH

#include "ChannelType.hh"
#include <iostream>

class ChannelInfo {
public:
  ChannelInfo(uint8_t const time_slot, uint32_t const frame_number);
  std::ostream &print(std::ostream &out) const;

  CCT channel_type() const;
  
private:
  CCT const m_cct;
  uint8_t const m_sub_slot;
  uint8_t const m_burst_offset;
};

#endif
