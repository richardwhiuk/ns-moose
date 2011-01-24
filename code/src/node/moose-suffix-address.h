/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Richard Whitehouse <ns3@richardwhiuk.com>
 */
#ifndef MOOSE_SUFFIX_ADDRESS_H
#define MOOSE_SUFFIX_ADDRESS_H

#include <string.h>
#include <ostream>
#include <stdint.h>

namespace ns3 {

class MooseSuffixAddress {

public:
  MooseSuffixAddress();
  MooseSuffixAddress(const uint8_t buffer[3]);

private:

  uint8_t m_address[6];

  friend bool operator < (const MooseSuffixAddress &a, const MooseSuffixAddress &b);
  friend bool operator == (const MooseSuffixAddress &a, const MooseSuffixAddress &b);
  friend bool operator != (const MooseSuffixAddress &a, const MooseSuffixAddress &b);
  friend std::istream& operator>> (std::istream& is, MooseSuffixAddress & address);

};

/**
 * \class ns3::Mac48AddressValue
 * \brief hold objects of type ns3::Mac48Address
 */

inline bool operator == (const MooseSuffixAddress &a, const MooseSuffixAddress &b)
{
  return memcmp (a.m_address, b.m_address, 3) == 0;
}
inline bool operator != (const MooseSuffixAddress &a, const MooseSuffixAddress &b)
{
  return memcmp (a.m_address, b.m_address, 3) != 0;
}
inline bool operator < (const MooseSuffixAddress &a, const MooseSuffixAddress &b)
{
  return memcmp (a.m_address, b.m_address, 3) < 0;
}

std::istream& operator>> (std::istream& is, MooseSuffixAddress & address);

} // namespace ns3

#endif
