/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Richard Whitehouse
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

#include "bpdu-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("TcnBpduHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(TcnBpduHeader);

TypeId TcnBpduHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcnBpduHeader")
    .SetParent<Header> ()
    .AddConstructor<TcnBpduHeader> ()
    ;
  return tid;
}

TypeId TcnBpduHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void TcnBpduHeader::Print (std::ostream &os) const
{
	os << "topology change notification "
	  ;
}

uint32_t TcnBpduHeader::GetSerializedSize (void) const
{
  /* this is the size of a Tcn BPDU excluding the BPDU Header. */
  return 0;
}

void TcnBpduHeader::Serialize (Buffer::Iterator start) const
{

}
uint32_t TcnBpduHeader::Deserialize (Buffer::Iterator start)
{
  return GetSerializedSize ();
}

}

