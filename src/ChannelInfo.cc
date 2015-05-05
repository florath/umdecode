#include "ChannelInfo.hh"
#include <cassert>

uint8_t ChannelInfo::burst_offset() const { return m_burst_offset; }

namespace {

struct CI {
  uint8_t fnmod;
  CCT cct;
  uint8_t offset;
  uint8_t sub_chan;
};

// See GSM 05.02 Section 7 Table 3
CI channel_type_lookup_array_ts0[51] = {
    /* TS = 0 */
    {0, CCT::FCCH, 0, 0},
    {1, CCT::SCH, 0, 0},
    {2, CCT::BCCH, 0, 0},
    {3, CCT::BCCH, 1, 0},
    {4, CCT::BCCH, 2, 0},
    {5, CCT::BCCH, 3, 0},
    {6, CCT::CCCH, 0, 0},
    {7, CCT::CCCH, 1, 0},
    {8, CCT::CCCH, 2, 0},
    {9, CCT::CCCH, 3, 0},
    {10, CCT::FCCH, 0, 0},
    {11, CCT::SCH, 0, 0},
    {12, CCT::CCCH, 0, 1},
    {13, CCT::CCCH, 1, 1},
    {14, CCT::CCCH, 2, 1},
    {15, CCT::CCCH, 3, 1},
    {16, CCT::CCCH, 0, 2},
    {17, CCT::CCCH, 1, 2},
    {18, CCT::CCCH, 2, 2},
    {19, CCT::CCCH, 3, 2},
    {20, CCT::FCCH, 0, 0},
    {21, CCT::SCH, 0, 0},
    {22, CCT::CCCH, 0, 3},
    {23, CCT::CCCH, 1, 3},
    {24, CCT::CCCH, 2, 3},
    {25, CCT::CCCH, 3, 3},
    {26, CCT::CCCH, 0, 4},
    {27, CCT::CCCH, 1, 4},
    {28, CCT::CCCH, 2, 4},
    {29, CCT::CCCH, 3, 4},
    {30, CCT::FCCH, 0, 0},
    {31, CCT::SCH, 0, 0},
    {32, CCT::CCCH, 0, 5},
    {33, CCT::CCCH, 1, 5},
    {34, CCT::CCCH, 2, 5},
    {35, CCT::CCCH, 3, 5},
    {36, CCT::CCCH, 0, 6},
    {37, CCT::CCCH, 1, 6},
    {38, CCT::CCCH, 2, 6},
    {39, CCT::CCCH, 3, 6},
    {40, CCT::FCCH, 0, 0},
    {41, CCT::SCH, 0, 0},
    {42, CCT::CCCH, 0, 7},
    {43, CCT::CCCH, 1, 7},
    {44, CCT::CCCH, 2, 7},
    {45, CCT::CCCH, 3, 7},
    {46, CCT::CCCH, 0, 8},
    {47, CCT::CCCH, 1, 8},
    {48, CCT::CCCH, 2, 8},
    {49, CCT::CCCH, 3, 8},
    {50, CCT::UNKNOWN, 0, 0}};

CI channel_type_lookup_array_tsN[51] = {
    /* TS = 1..7 */
    {0, CCT::SDCCH, 0, 0},
    {1, CCT::SDCCH, 1, 0},
    {2, CCT::SDCCH, 2, 0},
    {3, CCT::SDCCH, 3, 0},
    {4, CCT::SDCCH, 0, 1},
    {5, CCT::SDCCH, 1, 1},
    {6, CCT::SDCCH, 2, 1},
    {7, CCT::SDCCH, 3, 1},
    {8, CCT::SDCCH, 0, 2},
    {9, CCT::SDCCH, 1, 2},
    {10, CCT::SDCCH, 2, 2},
    {11, CCT::SDCCH, 3, 2},
    {12, CCT::SDCCH, 0, 3},
    {13, CCT::SDCCH, 1, 3},
    {14, CCT::SDCCH, 2, 3},
    {15, CCT::SDCCH, 3, 3},
    {16, CCT::SDCCH, 0, 4},
    {17, CCT::SDCCH, 1, 4},
    {18, CCT::SDCCH, 2, 4},
    {19, CCT::SDCCH, 3, 4},
    {20, CCT::SDCCH, 0, 5},
    {21, CCT::SDCCH, 1, 5},
    {22, CCT::SDCCH, 2, 5},
    {23, CCT::SDCCH, 3, 5},
    {24, CCT::SDCCH, 0, 6},
    {25, CCT::SDCCH, 1, 6},
    {26, CCT::SDCCH, 2, 6},
    {27, CCT::SDCCH, 3, 6},
    {28, CCT::SDCCH, 0, 7},
    {29, CCT::SDCCH, 1, 7},
    {30, CCT::SDCCH, 2, 7},
    {31, CCT::SDCCH, 3, 7},
    {32, CCT::SACCH, 0, 0 + 100},
    {33, CCT::SACCH, 1, 0 + 100},
    {34, CCT::SACCH, 2, 0 + 100},
    {35, CCT::SACCH, 3, 0 + 100},
    {36, CCT::SACCH, 0, 1 + 100},
    {37, CCT::SACCH, 1, 1 + 100},
    {38, CCT::SACCH, 2, 1 + 100},
    {39, CCT::SACCH, 3, 1 + 100},
    {40, CCT::SACCH, 0, 2 + 100},
    {41, CCT::SACCH, 1, 2 + 100},
    {42, CCT::SACCH, 2, 2 + 100},
    {43, CCT::SACCH, 3, 2 + 100},
    {44, CCT::SACCH, 0, 3 + 100},
    {45, CCT::SACCH, 1, 3 + 100},
    {46, CCT::SACCH, 2, 3 + 100},
    {47, CCT::SACCH, 3, 3 + 100},
    {48, CCT::UNKNOWN, 0, 0},
    {49, CCT::UNKNOWN, 0, 0},
    {50, CCT::UNKNOWN, 0, 0}};

CCT channel_type_lookup(uint8_t const time_slot, uint32_t const frame_number) {
  assert(time_slot < 8);

  if (time_slot == 0) {
    //    assert(channel_type_lookup_array_ts0[frame_number % 51].cct !=
    //           CCT::UNKNOWN);
    return channel_type_lookup_array_ts0[frame_number % 51].cct;
  }
  //  assert(channel_type_lookup_array_tsN[frame_number % 51].cct !=
  //  CCT::UNKNOWN);
  return channel_type_lookup_array_tsN[frame_number % 51].cct;
}

uint8_t sub_slot_lookup(uint8_t const time_slot, uint32_t const frame_number) {
  assert(time_slot < 8);

  if (time_slot == 0) {
    return channel_type_lookup_array_ts0[frame_number % 51].sub_chan;
  }

  uint8_t const rval(channel_type_lookup_array_tsN[frame_number % 51].sub_chan);
  // Normal sub slots.
  if (rval < 100) {
    return rval;
  }
  // If larger than 100, use frame number to get the mod 102
  if (frame_number % 102 > 50) {
    return rval - 100 + 4;
  }
  return rval - 100;
}

uint8_t burst_offset_lookup(uint8_t const time_slot,
                            uint32_t const frame_number) {
  assert(time_slot < 8);
  if (time_slot == 0) {
    return channel_type_lookup_array_ts0[frame_number % 51].offset;
  }
  return channel_type_lookup_array_tsN[frame_number % 51].offset;
}
}

ChannelInfo::ChannelInfo(uint8_t const time_slot, uint32_t const frame_number)
    : m_cct(channel_type_lookup(time_slot, frame_number)),
      m_sub_slot(sub_slot_lookup(time_slot, frame_number)),
      m_burst_offset(burst_offset_lookup(time_slot, frame_number)) {}

CCT ChannelInfo::channel_type() const { return m_cct; }

std::ostream &ChannelInfo::print(std::ostream &out) const {
  out << "[" << m_cct << "," << (int)m_sub_slot << "," << (int)m_burst_offset
      << "]";
  return out;
}

uint8_t ChannelInfo::sub_slot() const {
   return m_sub_slot;
}
