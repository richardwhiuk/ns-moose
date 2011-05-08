/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
#include "ethernet-bridge-net-device.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("EthernetBridgePortNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (EthernetBridgePortNetDevice);

TypeId
EthernetBridgePortNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EthernetBridgePortNetDevice")
    .SetParent<BridgePortNetDevice> ()
    ;
  return tid;
}


EthernetBridgePortNetDevice::EthernetBridgePortNetDevice(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node) : BridgePortNetDevice(bridge,device,node) {
  NS_LOG_FUNCTION_NOARGS ();
}

EthernetBridgePortNetDevice::~EthernetBridgePortNetDevice(){
  NS_LOG_FUNCTION_NOARGS ();
}

void EthernetBridgePortNetDevice::SetEnabled(bool enabled){
	m_enabled = enabled;
}

void EthernetBridgePortNetDevice::Send(Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber){
	NS_LOG_FUNCTION_NOARGS ();
	BridgePortNetDevice::Send(packet, source, dest, protocolNumber);
}

/**
 * By moving learning to the port, we can have some ports which learn, and some which don't.
 **/

void
EthernetBridgePortNetDevice::Receive (Ptr<NetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol,
                                    Address const &src, Address const &dst, NetDevice::PacketType packetType)
{
  NS_LOG_FUNCTION_NOARGS ();

  Ptr<EthernetBridgeNetDevice> bridge = m_bridge->GetObject<EthernetBridgeNetDevice>();

  if(m_enabled){

	NS_LOG_LOGIC ("UID is " << packet->GetUid ());

	NS_LOG_LOGIC ("LearningBridgeForward (incomingPort=" << incomingPort->GetInstanceTypeId ().GetName ()
                << ", packet=" << packet << ", protocol="<<protocol
                << ", src=" << src << ", dst=" << dst << ")");

	bridge->Learn(src, this);

	bridge->Forward(this, packet, protocol, src, dst, packetType);

  }

}

}

