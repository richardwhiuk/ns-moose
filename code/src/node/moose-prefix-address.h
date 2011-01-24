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
#ifndef MOOSE_PREFIX_ADDRESS_H
#define MOOSE_PREFIX_ADDRESS_H

#include <ostream>
#include <string.h>
#include <stdint.h>

namespace ns3 {

class MoosePrefixAddress {

public:
  MoosePrefixAddress();
  MoosePrefixAddress(const uint8_t buffer[2]);
  const uint16_t GetInt() const;

private:

  uint8_t m_address[2];

  friend bool operator < (const MoosePrefixAddress &a, const MoosePrefixAddress &b);
  friend bool operator == (const MoosePrefixAddress &a, const MoosePrefixAddress &b);
  friend bool operator != (const MoosePrefixAddress &a, const MoosePrefixAddress &b);
  friend std::istream& operator>> (std::istream& is, MoosePrefixAddress & address);

  friend class MooseAddress;

};

/**
 * \class ns3::Mac48AddressValue
 * \brief hold objects of type ns3::Mac48Address
 */

inline bool operator == (const MoosePrefixAddress &a, const MoosePrefixAddress &b)
{
  return memcmp (a.m_address, b.m_address, 2) == 0;
}
inline bool operator != (const MoosePrefixAddress &a, const MoosePrefixAddress &b)
{
  return memcmp (a.m_address, b.m_address, 2) != 0;
}
inline bool operator < (const MoosePrefixAddress &a, const MoosePrefixAddress &b)
{
  return memcmp (a.m_address, b.m_address, 2) < 0;
}

std::istream& operator>> (std::istream& is, MoosePrefixAddress & address);

} // namespace ns3


#endif
