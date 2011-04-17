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
#include "ns3/address-utils.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ConfigurationBpduHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(ConfigurationBpduHeader);

uint8_t ConfigurationBpduHeader::GetFlags(){
	return m_flags;
}

BpduBridgeIdentifier ConfigurationBpduHeader::GetRoot(){
	return m_root;
}

uint32_t ConfigurationBpduHeader::GetRootCost(){
	return m_rootPath;
}

BpduBridgeIdentifier ConfigurationBpduHeader::GetBridge(){
	return m_bridge;
}

uint32_t ConfigurationBpduHeader::GetBridgeCost(){
	return m_bridgePath;
}

uint16_t ConfigurationBpduHeader::GetPort(){
	return m_port;
}

uint16_t ConfigurationBpduHeader::GetMessageAge(){
	return m_messageAge;
}

uint16_t ConfigurationBpduHeader::GetMaxAge(){
	return m_maxAge;
}

uint16_t ConfigurationBpduHeader::GetHelloTime(){
	return m_helloTime;
}

uint16_t ConfigurationBpduHeader::GetForwardDelay(){
	return m_forwardDelay;
}

void ConfigurationBpduHeader::SetFlags(uint8_t flags){
	m_flags = flags;
}

void ConfigurationBpduHeader::SetRoot(BpduBridgeIdentifier root, uint32_t rootPath){
	m_root = root; 
	m_rootPath = rootPath;
}

void ConfigurationBpduHeader::SetBridge(BpduBridgeIdentifier bridge, uint32_t bridgePath){
	m_bridge = bridge;
	m_bridgePath = bridgePath;
}

void ConfigurationBpduHeader::SetPort(uint16_t port){
	m_port = port;
}

void ConfigurationBpduHeader::SetMessageAge(uint16_t messageAge){
	m_messageAge = messageAge;
}

void ConfigurationBpduHeader::SetMaxAge(uint16_t maxAge){
	m_maxAge = maxAge;
}

void ConfigurationBpduHeader::SetHelloTime(uint16_t helloTime){
	m_helloTime = helloTime;
}

void ConfigurationBpduHeader::SetForwardDelay(uint16_t forwardDelay){
	m_forwardDelay = forwardDelay;
}

TypeId ConfigurationBpduHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ConfigurationBpduHeader")
    .SetParent<BpduHeader> ()
    .AddConstructor<ConfigurationBpduHeader> ()
    ;
  return tid;
}

TypeId ConfigurationBpduHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void ConfigurationBpduHeader::Print (std::ostream &os) const
{
	os << "configuration "
	   << "flags: " << m_flags << " "
	   << "root: " << m_root.priority << " " << m_root.address
	   << "root path cost: " << m_rootPath << " "
	   << "bridge: " << m_bridge.priority << " " << m_bridge.address
	   << "bridge path cost: " << m_bridgePath << " "
	   << "port: " << m_port << " "
	   << "age: " << m_messageAge << "/" << m_maxAge << " "
	   << "hello: " << m_helloTime << " "
	   << "forward: " << m_forwardDelay << " "
	  ;
}

uint32_t ConfigurationBpduHeader::GetSerializedSize (void) const
{
  /* this is the size of a Configuration BPDU excluding the BPDU Header. */
  return 31;
}

void ConfigurationBpduHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_flags);
  i.WriteHtonU16 (m_root.priority);
  WriteTo(i, m_root.address);
  i.WriteHtonU32 (m_rootPath);
  i.WriteHtonU16 (m_bridge.priority);
  WriteTo(i, m_bridge.address);
  i.WriteHtonU32 (m_bridgePath);
  i.WriteHtonU16 (m_port);
  i.WriteHtonU16 (m_messageAge);
  i.WriteHtonU16 (m_maxAge);
  i.WriteHtonU16 (m_helloTime);
  i.WriteHtonU16 (m_forwardDelay);
}
uint32_t
ConfigurationBpduHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_flags = i.ReadU8 ();
  m_root.priority = i.ReadNtohU16 ();
  ReadFrom (i, m_root.address);
  m_rootPath = i.ReadNtohU32 ();
  m_bridge.priority = i.ReadNtohU16 ();
  ReadFrom (i, m_bridge.address);
  m_bridgePath = i.ReadNtohU32 ();
  m_port = i.ReadNtohU16 ();
  m_messageAge = i.ReadNtohU16 ();
  m_maxAge = i.ReadNtohU16 ();
  m_helloTime = i.ReadNtohU16 ();
  m_forwardDelay = i.ReadNtohU16 ();

  return GetSerializedSize ();
}


}


