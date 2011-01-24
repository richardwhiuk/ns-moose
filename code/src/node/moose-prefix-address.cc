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

#include "moose-prefix-address.h"

namespace ns3 {

MoosePrefixAddress::MoosePrefixAddress (){
   memset (m_address, 0, 2);
}

MoosePrefixAddress::MoosePrefixAddress(const uint8_t buffer[2]){
  memcpy(m_address, buffer, 2);
}

const uint16_t MoosePrefixAddress::GetInt() const{
  uint16_t i;
  i = m_address[0];
  i += m_address[1] << 8;
  return i;
}


}
