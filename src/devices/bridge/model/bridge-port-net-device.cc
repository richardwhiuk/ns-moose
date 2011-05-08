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
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("BridgePortNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BridgePortNetDevice);

TypeId
BridgePortNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BridgePortNetDevice")
    .SetParent<Object> ()
    ;
  return tid;
}

void BridgePortNetDevice::Send(Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber){
        NS_LOG_FUNCTION_NOARGS (); 
        if(m_enabled){
                NS_LOG_LOGIC("SendingPacket(src=" << source << ", dest=" << dest << ")");
                m_device->SendFrom(packet->Copy(), source, dest, protocolNumber);
        }   
}


BridgePortNetDevice::BridgePortNetDevice(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node) : m_bridge(bridge), m_device(device), m_enabled(true){
  NS_LOG_FUNCTION_NOARGS ();

  if (!Mac48Address::IsMatchingType (device->GetAddress()))
    {
      NS_FATAL_ERROR ("Device does not support eui 48 addresses: cannot be added to bridge.");
    }
  if (!device->SupportsSendFrom ())
    {
      NS_FATAL_ERROR ("Device does not support SendFrom: cannot be added to bridge.");
    }

  NS_LOG_DEBUG ("RegisterProtocolHandler for " << device->GetInstanceTypeId ().GetName ());

  node->RegisterProtocolHandler (MakeCallback (&BridgePortNetDevice::Receive, this), 0, device, true);

}

BridgePortNetDevice::~BridgePortNetDevice(){
  NS_LOG_FUNCTION_NOARGS ();
}

void BridgePortNetDevice::SetEnabled(bool enabled){
	m_enabled = enabled;
}

Ptr<NetDevice> BridgePortNetDevice::GetDevice()
{
	return m_device;
}

}

