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

#include "moose-suffix-address.h"

namespace ns3 {

MooseSuffixAddress::MooseSuffixAddress (){
   memset (m_address, 0, 6);
}

MooseSuffixAddress::MooseSuffixAddress(const uint8_t buffer[3]){
  memcpy(m_address, buffer, 3);
}

uint32_t MooseSuffixAddress::GetInt() const {
  uint32_t val = 0;
  val = m_address[0];
  val += m_address[1] << 8;
  val += m_address[2] << 16;
  return val;
}

}
