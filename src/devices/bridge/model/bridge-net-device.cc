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
#include "bridge-net-device.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("BridgeNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BridgeNetDevice);


TypeId
BridgeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BridgeNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<BridgeNetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (1500),
                   MakeUintegerAccessor (&BridgeNetDevice::SetMtu,
                                         &BridgeNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())    
    .AddAttribute ("EnableLearning",
                   "Enable the learning mode of the Learning Bridge",
                   BooleanValue (true),
                   MakeBooleanAccessor (&BridgeNetDevice::m_enableLearning),
                   MakeBooleanChecker ())
    ;
  return tid;
}


BridgeNetDevice::BridgeNetDevice ()
  : m_node (0),
    m_ifIndex (0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_channel = CreateObject<BridgeChannel> ();
  m_state = CreateObject<BridgeState> ();
}

BridgeNetDevice::~BridgeNetDevice()
{
  NS_LOG_FUNCTION_NOARGS ();
}

  void 
BridgeNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();
  for (std::vector< Ptr<BridgePortNetDevice> >::iterator iter = m_ports.begin (); iter != m_ports.end (); iter++)
    {
      *iter = 0;
    }
  m_ports.clear ();
  m_channel = 0;
  m_node = 0;
  NetDevice::DoDispose ();
}

void BridgeNetDevice::Forward (Ptr<BridgePortNetDevice> port, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, NetDevice::PacketType packetType){

  Mac48Address src48 = Mac48Address::ConvertFrom (src);
  Mac48Address dst48 = Mac48Address::ConvertFrom (dst);

  if (!m_promiscRxCallback.IsNull ())
    {
      m_promiscRxCallback (this, packet, protocol, src, dst, packetType);
    }

  switch (packetType)
    {
    case PACKET_HOST:
      if (dst48 == m_address)
        {
          m_rxCallback (this, packet, protocol, src);
        }
      break;

    case PACKET_BROADCAST:
    case PACKET_MULTICAST:
      m_rxCallback (this, packet, protocol, src);
      ForwardBroadcast (port, packet, protocol, src48, dst48);
      break;

    case PACKET_OTHERHOST:
      if (dst48 == m_address)
        {
          m_rxCallback (this, packet, protocol, src);
        }
      else
        {
          ForwardUnicast (port, packet, protocol, src48, dst48);
        }
      break;
    }
}

void
BridgeNetDevice::ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();

  Ptr<BridgePortNetDevice> outPort = GetLearnedState (dst);
  if (outPort != NULL && outPort != incomingPort)
    {
      NS_LOG_LOGIC ("Learning bridge state says to use port `" << outPort->GetInstanceTypeId ().GetName () << "'");
      outPort->Send (packet->Copy (), src, dst, protocol);
    }
  else
    {
      NS_LOG_LOGIC ("No learned state: send through all ports");
      for (std::vector< Ptr<BridgePortNetDevice> >::iterator iter = m_ports.begin ();
           iter != m_ports.end (); iter++)
        {
          Ptr<BridgePortNetDevice> port = *iter;
          if (port != incomingPort)
            {
              NS_LOG_LOGIC ("LearningBridgeForward (" << src << " => " << dst << "): " 
                            << incomingPort->GetInstanceTypeId ().GetName ()
                            << " --> " << port->GetInstanceTypeId ().GetName ()
                            << " (UID " << packet->GetUid () << ").");
              port->Send (packet->Copy (), src, dst, protocol);
            }
        }
    }
}

void
BridgeNetDevice::ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();

  for (std::vector< Ptr<BridgePortNetDevice> >::iterator iter = m_ports.begin ();
         iter != m_ports.end (); iter++)
    {
      Ptr<BridgePortNetDevice> port = *iter;
      if (port != incomingPort)
        {
          port->Send(packet->Copy (), src, dst, protocol);
        }
    }
}

void BridgeNetDevice::Learn (Address const &src, Ptr<BridgePortNetDevice> port)
{
  NS_LOG_FUNCTION_NOARGS ();

  Mac48Address src48 = Mac48Address::ConvertFrom (src);

  if (m_enableLearning)
    {
	m_state->Learn(src48, port);
    }
}

Ptr<BridgePortNetDevice> BridgeNetDevice::GetLearnedState (Mac48Address source)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_enableLearning)
    {
	return m_state->GetPort(source);
    }
  return NULL;
}

uint32_t
BridgeNetDevice::GetNBridgePorts (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ports.size ();
}

/** Dislike this slightly less now. */

Ptr<NetDevice>
BridgeNetDevice::GetBridgePort (uint32_t n) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ports[n]->GetDevice();
}

Ptr<BridgePortNetDevice>  
BridgeNetDevice::AddBridgePort (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (device != this);


  NS_LOG_DEBUG("AddBridgePort(bridge_addr=" << m_address << ", device_address=" << (Mac48Address::ConvertFrom(device->GetAddress())));

  if (m_address == Mac48Address ())
    {
      m_address = Mac48Address::ConvertFrom (device->GetAddress ());
    }

  Ptr<BridgePortNetDevice> port = CreateBridgePort(this, device, m_node);

  m_ports.push_back(port);
  m_channel->AddChannel (device->GetChannel ());

  return port;

}

Ptr<BridgePortNetDevice>
BridgeNetDevice::CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node){
	return CreateObject<BridgePortNetDevice>(bridge, device, node);
}

void 
BridgeNetDevice::SetIfIndex(const uint32_t index)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_ifIndex = index;
}

uint32_t 
BridgeNetDevice::GetIfIndex(void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_ifIndex;
}

Ptr<Channel> 
BridgeNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_channel;
}

void
BridgeNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_address = Mac48Address::ConvertFrom (address);
}

Address 
BridgeNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_address;
}

bool 
BridgeNetDevice::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_mtu = mtu;
  return true;
}

uint16_t 
BridgeNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mtu;
}

bool 
BridgeNetDevice::SetMaxStateSize (const unsigned long maxStateSize )
{
  NS_LOG_FUNCTION_NOARGS ();
  m_state->SetMaxSize(maxStateSize);
  return true;
}

unsigned long 
BridgeNetDevice::GetMaxStateSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_state->GetMaxSize();
}

unsigned long 
BridgeNetDevice::GetStateSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_state->GetSize();
}

bool 
BridgeNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


void 
BridgeNetDevice::AddLinkChangeCallback (Callback<void> callback)
{}


bool 
BridgeNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


Address
BridgeNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
BridgeNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address
BridgeNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
 NS_LOG_FUNCTION (this << multicastGroup);
 Mac48Address multicast = Mac48Address::GetMulticast (multicastGroup);
 return multicast;
}


bool 
BridgeNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return false;
}

bool 
BridgeNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


bool 
BridgeNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  return SendFrom (packet, m_address, dest, protocolNumber);
}

bool 
BridgeNetDevice::SendFrom (Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber)
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


Ptr<Node> 
BridgeNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_node;
}


void 
BridgeNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = node;
}


bool 
BridgeNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}


void 
BridgeNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_rxCallback = cb;
}

void 
BridgeNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_promiscRxCallback = cb;
}

bool
BridgeNetDevice::SupportsSendFrom () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address BridgeNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  return Mac48Address::GetMulticast (addr);
}

std::ostream& BridgeNetDevice::Print(std::ostream& file){
	file << 1 << std::endl;
	file << m_address << std::endl;
	file << *m_state;
	return file;
}

std::ostream& operator<<(std::ostream& file, BridgeNetDevice& dev){
	return dev.Print(file);
}

} // namespace ns3
