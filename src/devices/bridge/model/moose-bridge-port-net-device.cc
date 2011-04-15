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
#include "ns3/arp-header.h"
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

  Ptr<MooseBridgeNetDevice> bridge = m_bridge->GetObject<MooseBridgeNetDevice>();

  NS_ASSERT(bridge != NULL);

     Ptr<Packet> npacket = packet->Copy();

     if(protocolNumber == ArpL3Protocol::PROT_NUMBER){
        // Need to do ARP rewriting.

        ArpHeader arpHeader;
        npacket->RemoveHeader(arpHeader);

        Mac48Address arp_dst48 = Mac48Address::ConvertFrom(arpHeader.GetDestinationHardwareAddress());
        MooseAddress arp_mdst(arp_dst48);

        if(arp_mdst.GetMooseType() == MooseAddress::MOOSE){
                if(arpHeader.IsRequest()){
                        arpHeader.SetRequest(
                                arpHeader.GetSourceHardwareAddress(),
                                arpHeader.GetSourceIpv4Address(),
                                bridge->FromMoose(arp_mdst).GetMacAddress(),
                                arpHeader.GetDestinationIpv4Address()
                        );
                } else if(arpHeader.IsReply()){
                        arpHeader.SetReply(
                                arpHeader.GetSourceHardwareAddress(),
                                arpHeader.GetSourceIpv4Address(),
                                bridge->FromMoose(arp_mdst).GetMacAddress(),
                                arpHeader.GetDestinationIpv4Address()
                        );
                } else {
                        NS_ASSERT (arpHeader.IsReply() || arpHeader.IsRequest());
                }

            NS_LOG_LOGIC("Rewriting ARP Header Destination (from=" << arp_dst48 << ",to=" << bridge->FromMoose(arp_mdst).GetMacAddress() << ")");

        }

        npacket->AddHeader(arpHeader);

     }
 
  BridgePortNetDevice::Send(npacket, src, dest, protocolNumber);
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

     Ptr<Packet> npacket = packet->Copy();

     if(protocol == ArpL3Protocol::PROT_NUMBER){
        // Need to do ARP rewriting.

        ArpHeader arpHeader;
        npacket->RemoveHeader(arpHeader);
    
        Mac48Address arp_src48 = Mac48Address::ConvertFrom(arpHeader.GetSourceHardwareAddress());
        MooseAddress arp_msrc(arp_src48);

	if(arp_msrc.GetMooseType() == MooseAddress::HOST){
		if(arpHeader.IsRequest()){
			arpHeader.SetRequest(
				bridge->ToMoose(arp_msrc).GetMacAddress(),
				arpHeader.GetSourceIpv4Address(),
				arpHeader.GetDestinationHardwareAddress(),
				arpHeader.GetDestinationIpv4Address()
			);	
		} else if(arpHeader.IsReply()){
                        arpHeader.SetReply(
                                bridge->ToMoose(arp_msrc).GetMacAddress(),
                                arpHeader.GetSourceIpv4Address(),
                                arpHeader.GetDestinationHardwareAddress(),
                                arpHeader.GetDestinationIpv4Address()       
                        );
		} else {
			NS_ASSERT (arpHeader.IsReply() || arpHeader.IsRequest());
		}
         
		NS_LOG_LOGIC("Rewriting ARP Header Source (from=" << arp_src48 << ",to=" << bridge->ToMoose(arp_msrc).GetMacAddress() << ")");       
	}

        npacket->AddHeader(arpHeader);

     }

     bridge->Learn(newsrc, this);

     bridge->Forward(this, npacket, protocol, newsrc.GetMacAddress(), dst, packetType);

  } else {

     NS_LOG_LOGIC("Multicast Packet Recieved");

     bridge->Forward(this, packet, protocol, src, dst, packetType);     

  }
}

}

