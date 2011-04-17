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

NS_LOG_COMPONENT_DEFINE("BpduHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(BpduHeader);

void BpduHeader::SetProtocol(uint16_t protocolIdentifier){
	m_protocolIdentifier = protocolIdentifier;
}

void BpduHeader::SetVersion(uint8_t versionIdentifier){
	m_versionIdentifier = versionIdentifier;
}

void BpduHeader::SetType(uint8_t bpduType){
	m_bpduType = bpduType;
}

uint16_t BpduHeader::GetProtocol(){
	return m_protocolIdentifier;
}

uint8_t BpduHeader::GetVersion(){
	return m_versionIdentifier;
}

uint8_t BpduHeader::GetType(){
	return m_bpduType;
}

TypeId BpduHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BpduHeader")
    .SetParent<Header> ()
    .AddConstructor<BpduHeader> ()
    ;
  return tid;
}

TypeId BpduHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void BpduHeader::Print (std::ostream &os) const
{
	os << "bpdu "
	   << "protocol: " << m_protocolIdentifier << " "
	   << "version: " << m_versionIdentifier << " "
	   << "type: " << m_bpduType << " "
	  ;
}

uint32_t BpduHeader::GetSerializedSize (void) const
{
  /* this is the size of a BPDU Header excluding the Type. */
  return 4;
}

void BpduHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU16 (m_protocolIdentifier);
  i.WriteU8 (m_versionIdentifier);
  i.WriteU8 (m_bpduType);
}
uint32_t
BpduHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_protocolIdentifier = i.ReadNtohU16 ();
  m_versionIdentifier = i.ReadU8 ();
  m_bpduType = i.ReadU8 ();

  return GetSerializedSize ();
}


}

