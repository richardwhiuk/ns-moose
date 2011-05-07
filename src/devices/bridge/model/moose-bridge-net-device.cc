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
#include "ns3/simulator.h"
#include "ns3/packet.h"

NS_LOG_COMPONENT_DEFINE ("MooseBridgeNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MooseBridgeNetDevice);

TypeId
MooseBridgeNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MooseBridgeNetDevice")
    .SetParent<BridgeNetDevice> ()
    .AddConstructor<MooseBridgeNetDevice> ()
    .AddAttribute ("MooseAddress",
		   "Bridge MOOSE Address",
		   MooseAddressValue(),
		   MakeMooseAddressAccessor (&MooseBridgeNetDevice::m_mooseAddress),
		   MakeMooseAddressChecker ())
	;
  return tid;
}

MooseBridgeNetDevice::MooseBridgeNetDevice() : BridgeNetDevice() {
  NS_LOG_FUNCTION_NOARGS ();

  m_mooseAddress = MooseAddress::Allocate();
  m_state = CreateObject<MooseBridgeState> ();

}

MooseBridgeNetDevice::~MooseBridgeNetDevice(){
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<BridgePortNetDevice> MooseBridgeNetDevice::CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node){
	return CreateObject<MooseBridgePortNetDevice>(bridge, device, node);
}

MooseAddress MooseBridgeNetDevice::FromMoose(MooseAddress const& addr){
  NS_LOG_FUNCTION_NOARGS();

  if(addr.GetMooseType() != MooseAddress::MOOSE){
     return MooseAddress(Mac48Address::GetBroadcast());
  }
  
  // Don't translate none local MOOSE addrs.

  if(addr.GetMoosePrefix() != m_mooseAddress.GetMoosePrefix()){
     return addr;
  }

  return m_state->GetMac(addr.GetMooseSuffix());
 
}

MooseAddress MooseBridgeNetDevice::ToMoose(MooseAddress const& addr){
  NS_LOG_FUNCTION_NOARGS();

  MooseAddress::MooseType mt = addr.GetMooseType();

  if(mt == MooseAddress::MOOSE){ 
     return addr;
  } else if(mt == MooseAddress::HOST){
     MooseAddress moose;

     Mac48Address addr48 = addr.GetMacAddress();

     MooseSuffixAddress suffix = m_state->GetSuffix(addr48, m_mooseAddress.GetMoosePrefix());

     return MooseAddress::Combine(m_mooseAddress.GetMoosePrefix(), suffix);

  } else {
     NS_ASSERT( (mt == MooseAddress::HOST) || (mt == MooseAddress::MOOSE) );  
     return addr;
  }

}

void MooseBridgeNetDevice::AddRoutes(std::map<MoosePrefixAddress, Ptr<BridgePortNetDevice> > routes){
	NS_LOG_FUNCTION_NOARGS ();

	for(std::map<MoosePrefixAddress, Ptr<BridgePortNetDevice> >::iterator it = routes.begin(); it != routes.end(); it ++){
		
		m_state->Fixed(it->first, it->second);

	}

}

void MooseBridgeNetDevice::Learn(MooseAddress const& addr, Ptr<BridgePortNetDevice> port){
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT(addr.GetMooseType() == MooseAddress::MOOSE);

  if(addr.GetMoosePrefix() != m_mooseAddress.GetMoosePrefix()){
     
	m_state->Learn(addr.GetMoosePrefix(), port);

  } else {

	m_state->Learn(addr.GetMooseSuffix(), port);

  }
 
}

Ptr<BridgePortNetDevice> MooseBridgeNetDevice::GetLearnedPort(MooseAddress const&addr){
  NS_LOG_FUNCTION_NOARGS();

  if(addr.GetMooseType() != MooseAddress::MOOSE){
      return NULL;
  }

  if(addr.GetMoosePrefix() != m_mooseAddress.GetMoosePrefix()){
	return m_state->GetPort(addr.GetMoosePrefix());
  } else {
	return m_state->GetPort(addr.GetMooseSuffix());
  }
  
  return NULL;
}


void
MooseBridgeNetDevice::ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS (); 

  // Get Port

  MooseAddress mdst = MooseAddress(dst);

  Ptr<BridgePortNetDevice> outPort = GetLearnedPort(mdst);

  if (outPort != NULL && outPort != incomingPort){   
      NS_LOG_LOGIC ("Learning bridge state says to use port `" << outPort->GetInstanceTypeId ().GetName () << "'");
      
      Mac48Address newdst = FromMoose(mdst).GetMacAddress();
      
      if(!newdst.IsBroadcast()){

         NS_LOG_LOGIC("MOOSE bridge says MAC Address is `" << newdst << "'");

         outPort->Send (packet->Copy (), src, newdst, protocol);
         return;
      }   
  } else {
      Mac48Address newdst = FromMoose(mdst).GetMacAddress();
    
      if(!newdst.IsBroadcast()){

         NS_LOG_LOGIC("MOOSE bridge says MAC Address is `" << newdst << "' - no port data, so broadcasting."); 

	 ForwardBroadcast(incomingPort, packet, protocol, src, newdst);

         return;
      }
  }

  NS_LOG_LOGIC("Not enough learned state. Dropping packet");

}

void MooseBridgeNetDevice::ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)
{
  NS_LOG_FUNCTION_NOARGS ();

  // We need to do reverse path forwarding to prevent loops

  // This is as simple as finding out the best port for sending and checking for equality

  Ptr<BridgePortNetDevice> bestPort = GetLearnedPort(MooseAddress(src));

  if(bestPort == NULL){
      // Tricky. Best idea: Forward to all ports and hope that we can resolve later :)
  } else if(bestPort != incomingPort){
       return;
  } 

  // Now we can forward.

  BridgeNetDevice::ForwardBroadcast(incomingPort, packet, protocol, src, dst);

}


}

