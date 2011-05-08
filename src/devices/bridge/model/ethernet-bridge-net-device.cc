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
#include "ethernet-bridge-net-device.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("EthernetBridgeNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (EthernetBridgeNetDevice);


TypeId
EthernetBridgeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EthernetBridgeNetDevice")
    .SetParent<BridgeNetDevice> ()
    .AddConstructor<EthernetBridgeNetDevice> ()
    .AddAttribute ("EnableLearning",
                   "Enable the learning mode of the Learning Bridge",
                   BooleanValue (true),
                   MakeBooleanAccessor (&EthernetBridgeNetDevice::m_enableLearning),
                   MakeBooleanChecker ())
    ;
  return tid;
}


EthernetBridgeNetDevice::EthernetBridgeNetDevice () : BridgeNetDevice() 
{
  NS_LOG_FUNCTION_NOARGS ();
  m_state = CreateObject<EthernetBridgeState> ();
}

EthernetBridgeNetDevice::~EthernetBridgeNetDevice()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void EthernetBridgeNetDevice::Forward (Ptr<BridgePortNetDevice> port, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, NetDevice::PacketType packetType){

  Mac48Address src48 = Mac48Address::ConvertFrom (src);
  Mac48Address dst48 = Mac48Address::ConvertFrom (dst);

/*  if (!m_promiscRxCallback.IsNull ())
    {
      m_promiscRxCallback (this, packet, protocol, src, dst, packetType);
    } */

  switch (packetType)
    {
    case PACKET_HOST:
      if (dst48 == m_macAddress)
        {
//          m_rxCallback (this, packet, protocol, src);
        }
      break;

    case PACKET_BROADCAST:
    case PACKET_MULTICAST:
//      m_rxCallback (this, packet, protocol, src);
      ForwardBroadcast (port, packet, protocol, src48, dst48);
      break;

    case PACKET_OTHERHOST:
      if (dst48 == m_macAddress)
        {
//          m_rxCallback (this, packet, protocol, src);
        }
      else
        {
          ForwardUnicast (port, packet, protocol, src48, dst48);
        }
      break;
    }
}

void
EthernetBridgeNetDevice::ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)
{
	NS_LOG_FUNCTION_NOARGS ();

	Ptr<BridgePortNetDevice> outPort = GetLearnedState (dst);
	
	if (outPort != NULL && outPort != incomingPort){

		NS_LOG_LOGIC ("Learning bridge state says to use port `" << outPort->GetInstanceTypeId ().GetName () << "'");

		outPort->Send (packet->Copy (), src, dst, protocol);

	} else {

		NS_LOG_LOGIC ("No learned state: send through all ports");

		ForwardBroadcast(incomingPort, packet, protocol, src, dst);

	}
}

void EthernetBridgeNetDevice::ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)
{
	NS_LOG_FUNCTION_NOARGS();

	BridgeNetDevice::ForwardBroadcast(incomingPort, packet, protocol, src, dst);
}

void EthernetBridgeNetDevice::Learn (Address const &src, Ptr<BridgePortNetDevice> port){
	NS_LOG_FUNCTION_NOARGS ();

	Mac48Address src48 = Mac48Address::ConvertFrom (src);

  if (m_enableLearning)
    {
	m_state->Learn(src48, port);
    }
}

Ptr<BridgePortNetDevice> EthernetBridgeNetDevice::GetLearnedState (Mac48Address source)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_enableLearning)
    {
	return m_state->GetPort(source);
    }
  return NULL;
}

Ptr<BridgePortNetDevice>
EthernetBridgeNetDevice::CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node){
	return CreateObject<EthernetBridgePortNetDevice>(bridge, device, node);
}

/*
bool 
EthernetBridgeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  return SendFrom (packet, m_address, dest, protocolNumber);
}

bool 
EthernetBridgeNetDevice::SendFrom (Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  Mac48Address dst = Mac48Address::ConvertFrom (dest); 

  // try to use the learned state if data is unicast
  if (!dst.IsGroup ())
    {
      Ptr<BridgePortNetDevice> outPort = GetLearnedState (dst);
      if (outPort != NULL) 
        {
          outPort->Send(packet, src, dest, protocolNumber);
          return true;
        }
    }

  // data was not unicast or no state has been learned for that mac
  // address => flood through all ports.
  for (std::vector< Ptr<BridgePortNetDevice> >::iterator iter = m_ports.begin ();
       iter != m_ports.end (); iter++)
    {
      Ptr<BridgePortNetDevice> port = *iter;
      port->Send (packet, src, dest, protocolNumber);
    }

  return true;
}
*/

std::ostream& EthernetBridgeNetDevice::Print(std::ostream& file){
	file << 1 << std::endl;
	file << m_macAddress << std::endl;
	file << *m_state;
	return file;
}

} // namespace ns3
