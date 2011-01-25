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

#include "bridge-net-device.h"
#include "moose-bridge-net-device.h"
#include "bridge-port-net-device.h"
#include "moose-bridge-port-net-device.h"
#include "ns3/moose-address.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include "ns3/arp-l3-protocol.h"
#include <iostream>

NS_LOG_COMPONENT_DEFINE ("MooseBridgePortNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MooseBridgePortNetDevice);

TypeId
MooseBridgePortNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MooseBridgePortNetDevice")
    .SetParent<BridgePortNetDevice> ()
    ;
  return tid;
}

MooseBridgePortNetDevice::MooseBridgePortNetDevice(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node) : BridgePortNetDevice(bridge,device,node){
  NS_LOG_FUNCTION_NOARGS ();
}

MooseBridgePortNetDevice::~MooseBridgePortNetDevice(){
  NS_LOG_FUNCTION_NOARGS ();
}

void MooseBridgePortNetDevice::Send (Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber){
  NS_LOG_FUNCTION_NOARGS();
  NS_LOG_DEBUG("Send(device=" << m_device << ",packet=" << packet << ",src=" << src << ",dest=" << dest << ",port=" << this << ",protocol=" << protocolNumber);
  
  BridgePortNetDevice::Send(packet, src, dest, protocolNumber);
}

void MooseBridgePortNetDevice::Receive (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, NetDevice::PacketType packetType){
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_DEBUG("Receive(device=" << device << ",packet=" << packet << ",src=" << src << ",dst=" << dst << ",type=" << packetType << ",port=" << this << ",protocol=" << protocol << ")");

  Mac48Address src48 = Mac48Address::ConvertFrom(src);

  MooseAddress msrc(src48);

  Ptr<MooseBridgeNetDevice> bridge = m_bridge->GetObject<MooseBridgeNetDevice>();

  NS_ASSERT(bridge != NULL);

  MooseAddress::MooseType mt = msrc.GetMooseType();

  if(mt == MooseAddress::MOOSE){
     // We have a MOOSE Packet. Let it continue.

     NS_LOG_LOGIC("MOOSE Packet Received");

     bridge->Learn(msrc, this);

     bridge->Forward(this, packet, protocol, src, dst, packetType);

  } else if(mt == MooseAddress::HOST) {
     
     NS_LOG_LOGIC("Unicast Packet Received");

     MooseAddress newsrc = bridge->ToMoose(msrc);

     packet->Print(std::cout); std::cout << std::endl;
     if(protocol == ArpL3Protocol::PROT_NUMBER){
        // Need to do ARP rewriting.

     }

     bridge->Learn(newsrc, this);

     bridge->Forward(this, packet, protocol, newsrc.GetMacAddress(), dst, packetType);

  } else {

     NS_LOG_LOGIC("Multicast Packet Recieved");

     bridge->Forward(this, packet, protocol, src, dst, packetType);     

  }
}

}

