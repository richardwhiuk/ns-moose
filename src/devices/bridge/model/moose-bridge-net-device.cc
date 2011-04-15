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
    .AddAttribute ("MooseExpirationTime",
                   "Time it takes for learned MOOSE state entry to expire.",
                   TimeValue (Seconds (30)),
                   MakeTimeAccessor (&MooseBridgeNetDevice::m_expirationTime),
                   MakeTimeChecker ());
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

MooseAddress MooseBridgeNetDevice::FromMoose(MooseAddress const& addr){
  NS_LOG_FUNCTION_NOARGS();

  if(addr.GetMooseType() != MooseAddress::MOOSE){
     return MooseAddress(Mac48Address::GetBroadcast());
  }
  
  // Don't translate none local MOOSE addrs.

  if(addr.GetMoosePrefix() != m_mooseAddress.GetMoosePrefix()){
     return addr;
  }
 
  std::map<MooseSuffixAddress, SuffixState*>::iterator iter = m_ethernetState.find(addr.GetMooseSuffix());

  if(iter == m_ethernetState.end()){

     // Expired state - must broadcast.     

     return MooseAddress(Mac48Address::GetBroadcast());

  } else {

     Time now = Simulator::Now();     

     SuffixState* state = iter->second;
    
     if(state->expirationTime > now){ 
        return MooseAddress(iter->second->ethernet);
     } else {
        m_ethernetState.erase(iter);
        m_suffixState.erase(iter->second->ethernet);
        return MooseAddress(Mac48Address::GetBroadcast());
     }

  }
  
}

MooseAddress MooseBridgeNetDevice::ToMoose(MooseAddress const& addr){
  NS_LOG_FUNCTION_NOARGS();

  Time now = Simulator::Now();

  MooseAddress::MooseType mt = addr.GetMooseType();

  if(mt == MooseAddress::MOOSE){ 
     return addr;
  } else if(mt == MooseAddress::HOST){
     MooseAddress moose;

     Mac48Address addr48 = addr.GetMacAddress();

     // Fix Suffix.
     std::map<Mac48Address, SuffixState>::iterator iter = m_suffixState.find(addr48);
     if(iter == m_suffixState.end()){
         // Allocate New Suffix

         moose = MooseAddress::Allocate(m_mooseAddress.GetMoosePrefix()); 

         SuffixState &state = m_suffixState[addr48];
         state.ethernet = addr48;
         state.suffix = moose.GetMooseSuffix();
         state.expirationTime = now + m_expirationTime;

	 NS_LOG_LOGIC("Allocating New MOOSE Suffix: (" << moose.GetMoosePrefix().GetInt() << "," << moose.GetMooseSuffix().GetInt() << ") for " << addr48);

         m_ethernetState[state.suffix] = &state;

     } else {
        SuffixState &state = iter->second;
        if(state.expirationTime > now){
            moose = MooseAddress::Combine(m_mooseAddress.GetMoosePrefix(), state.suffix);

	    NS_LOG_LOGIC("Using Allocated MOOSE Suffix: (" << moose.GetMoosePrefix().GetInt() << "," << moose.GetMooseSuffix().GetInt() << ")");

            if(now + m_expirationTime > state.expirationTime){
	    	state.expirationTime = now + m_expirationTime;
            }
        } else {
            // Delete old and alloc new

            m_ethernetState.erase(state.suffix);
            m_suffixState.erase(iter);

            // Allocate New

            SuffixState &state = m_suffixState[addr48];
            
            moose = MooseAddress::Allocate(m_mooseAddress.GetMoosePrefix());
	    state.ethernet = addr48;
            state.suffix = moose.GetMooseSuffix();
            state.expirationTime = now + m_expirationTime;

	    NS_LOG_LOGIC("Expired MOOSE Suffix: (" << moose.GetMoosePrefix().GetInt() << "," << moose.GetMooseSuffix().GetInt() << ")");

            m_ethernetState[state.suffix] = &state;

        }
     }

     return moose;

  } else {
     NS_ASSERT( (mt == MooseAddress::HOST) || (mt == MooseAddress::MOOSE) );  
     return addr;
  }

}

void MooseBridgeNetDevice::AddRoutes(std::map<MoosePrefixAddress, Ptr<BridgePortNetDevice> > routes){
	NS_LOG_FUNCTION_NOARGS ();

	for(std::map<MoosePrefixAddress, Ptr<BridgePortNetDevice> >::iterator it = routes.begin(); it != routes.end(); it ++){
		
		NS_LOG_LOGIC("Adding Prefix State: (port: " << it->second << ", time: " << Simulator::GetMaximumSimulationTime() << ")");
		
     		PrefixState &state = m_prefixState[it->first];
     		state.associatedPort = it->second;
     		state.expirationTime = Simulator::GetMaximumSimulationTime();

	}

}

void MooseBridgeNetDevice::Learn(MooseAddress const& addr, Ptr<BridgePortNetDevice> port){
  NS_LOG_FUNCTION_NOARGS ();
  
  Time now = Simulator::Now();

  NS_ASSERT(addr.GetMooseType() == MooseAddress::MOOSE);

  if(addr.GetMoosePrefix() != m_mooseAddress.GetMoosePrefix()){
     
     PrefixState &state = m_prefixState[addr.GetMoosePrefix()];
     if(now + m_expirationTime > state.expirationTime){
	     NS_LOG_LOGIC("Updating Learned Prefix State: (port: " << port << ", time: " << now + m_expirationTime << ", address: " << addr << ") - Previous State: (port: " << state.associatedPort << ", time: " << state.expirationTime << ")");

	     state.associatedPort = port;				// Only update if the expiration is not in the future
	     state.expirationTime = now + m_expirationTime;
     }

  } else {

     PortState &state = m_portState[addr.GetMooseSuffix()];
     if(now + m_expirationTime > state.expirationTime){
	     NS_LOG_LOGIC("Updating Learned Suffix State: (port: " << port << ", time: " << now + m_expirationTime << ", address: " << addr << ") - Previous State: (port: " << state.associatedPort << ", time: " << state.expirationTime << ")");
	     state.associatedPort = port;
	     state.expirationTime = now + m_expirationTime;
     }

  }
 
}

Ptr<BridgePortNetDevice> MooseBridgeNetDevice::GetLearnedPort(MooseAddress const&addr){
  NS_LOG_FUNCTION_NOARGS();

  Time now = Simulator::Now();

  if(addr.GetMooseType() != MooseAddress::MOOSE){
      return NULL;
  }

  if(addr.GetMoosePrefix() != m_mooseAddress.GetMoosePrefix()){
     std::map<MoosePrefixAddress, PrefixState>::iterator iter = m_prefixState.find(addr.GetMoosePrefix());

     if(iter != m_prefixState.end()){
          return iter->second.associatedPort;
     }
  } else {
     std::map<MooseSuffixAddress, PortState>::iterator iter = m_portState.find(addr.GetMooseSuffix());

     if(iter != m_portState.end()){
          return iter->second.associatedPort;
     }
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

