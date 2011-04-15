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

#include "moose-address.h"
#include <map>

namespace ns3 {

MooseAddress::MooseAddress (){
   memset (m_address, 0, 6);
}

MooseAddress::MooseAddress (const char *str) {
   memset (m_address, 0, 6);
   Mac48Address mac(str);
   mac.CopyTo(m_address);
}

MooseAddress::MooseAddress (const Mac48Address &mac) {
   memset (m_address, 0, 6);
   mac.CopyTo(m_address);
}

const MoosePrefixAddress MooseAddress::GetMoosePrefix() const
{
   MoosePrefixAddress prefix(m_address + 1);  
   return prefix;
}

const MooseSuffixAddress MooseAddress::GetMooseSuffix() const
{
   MooseSuffixAddress suffix(m_address + 3);  
   return suffix;
}

const Mac48Address MooseAddress::GetMacAddress() const
{
   Mac48Address mac;
   mac.CopyFrom(m_address);
   return mac;
}

MooseAddress::MooseType MooseAddress::GetMooseType () const
{
  if((m_address[0] & 0x01) == 0x01){
	return GROUP;
  } else if((m_address[0] & 0x02) == 0x02){
	return MOOSE;
  } else {
        return HOST;
  }
}

MooseAddress MooseAddress::Allocate()
{
  static uint16_t id = 0;
  ++id;

  MooseAddress moose;

  moose.m_address[0] = 0 | 0x02;
  moose.m_address[1] = (id >> 8) & 0xff;
  moose.m_address[2] = (id >> 0) & 0xff;

  return moose;
}

MooseAddress MooseAddress::Allocate(MoosePrefixAddress prefix){

  static std::map<uint16_t, uint32_t> ids;

  uint16_t key = prefix.GetInt();

  uint32_t val = ++ids[key];

  MooseAddress moose;
  moose.m_address[0] = 0 | 0x02;			// MOOSE Address

  moose.m_address[1] = (key >>  8) & 0xff;		// Bridge
  moose.m_address[2] = (key >>  0) & 0xff;

  moose.m_address[3] = (val >> 16) & 0xff;		// Port
  moose.m_address[4] = (val >>  8) & 0xff;
  moose.m_address[5] = (val >>  0) & 0xff;

  return moose;

}

MooseAddress MooseAddress::Combine(MoosePrefixAddress prefix, MooseSuffixAddress suffix){

  uint16_t key = prefix.GetInt();
  uint32_t val = suffix.GetInt();

  MooseAddress moose;

  moose.m_address[0] = 0 | 0x02;                        // MOOSE Address

  moose.m_address[1] = (key >>  8) & 0xff;              // Bridge
  moose.m_address[2] = (key >>  0) & 0xff;

  moose.m_address[3] = (val >> 16) & 0xff;              // Port
  moose.m_address[4] = (val >>  8) & 0xff;
  moose.m_address[5] = (val >>  0) & 0xff;

  return moose;

}

ATTRIBUTE_HELPER_CPP (MooseAddress);

std::ostream& operator<< (std::ostream& os, MooseAddress const& address)
{
  os << (address.GetMacAddress());
  return os;
}
std::istream & operator >> (std::istream &is, MooseAddress &address)
{
  Mac48Address addr;
  is >> addr;
  address = MooseAddress(addr);
  return is;
}

}

