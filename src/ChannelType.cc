#include "ChannelType.hh"

std::ostream &operator<<(std::ostream &out, CCT const &cct) {
  switch (cct) {
  case CCT::UNKNOWN:
    out << "UNKNOWN";
    break;
  case CCT::FCCH:
    out << "FCCH ";
    break;
  case CCT::SCH:
    out << "SCH  ";
    break;
  case CCT::BCCH:
    out << "BCCH ";
    break;
  case CCT::CCCH:
    out << "CCCH ";
    break;
  case CCT::SDCCH:
    out << "SDCCH";
    break;
  case CCT::SACCH:
    out << "SACCH";
    break;
  }
  return out;
}
