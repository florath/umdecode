#ifndef CHANNEL_TYPE_HH
#define CHANNEL_TYPE_HH

#include <iostream>

// Control Channel Type
enum class CCT { UNKNOWN, FCCH, SCH, BCCH, CCCH, SDCCH, SACCH };

std::ostream &operator<<(std::ostream &out, CCT const &cct);

#endif
