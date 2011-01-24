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
 * Author: Gustavo Carneiro  <gjc@inescporto.pt>
 */

#include "moose-bridge-net-device.h"
#include "moose-bridge-port-net-device.h"
#include "ns3/log.h"
#include "ns3/node.h"

NS_LOG_COMPONENT_DEFINE ("MooseBridgeNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MooseBridgeNetDevice);

TypeId
MooseBridgeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MooseBridgeNetDevice")
    .SetParent<BridgeNetDevice> ()
    .AddConstructor<MooseBridgeNetDevice> ();
  return tid;
}

MooseBridgeNetDevice::MooseBridgeNetDevice() : BridgeNetDevice() {
  NS_LOG_FUNCTION_NOARGS ();

  m_mooseAddress = MooseAddress::Allocate();

}

MooseBridgeNetDevice::~MooseBridgeNetDevice(){
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<BridgePortNetDevice> MooseBridgeNetDevice::CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node){
	return CreateObject<MooseBridgePortNetDevice>(bridge, device, node);
}

/*
//bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
//bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);

void Forward (Ptr<BridgePortNetDevice> port, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, PacketType packetType){
	
}

void ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
void ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
void Learn (Address const &src, Ptr<BridgePortNetDevice> port);
Ptr<BridgePortNetDevice> GetLearnedState (Mac48Address source);*/


}

