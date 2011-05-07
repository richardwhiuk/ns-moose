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

#include "moose-bridge-state.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("MooseBridgeState");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MooseBridgeState);

TypeId MooseBridgeState::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::MooseBridgeState")
			.SetParent<Object> ()
			.AddConstructor<MooseBridgeState> ()
			.AddAttribute (	"ExpirationTime",
					"Time it takes for learned state entry to expire.",
					TimeValue (Seconds (300)),
					MakeTimeAccessor (&MooseBridgeState::m_time),
					MakeTimeChecker ())
			.AddAttribute (	"StateSize", "The size of the State Table",
					UintegerValue (8000),
					MakeUintegerAccessor (&MooseBridgeState::m_max),
					MakeUintegerChecker<unsigned long> ())
			;
	return tid;
}

MooseBridgeState::MooseBridgeState(){

}

MooseBridgeState::~MooseBridgeState(){

}

void MooseBridgeState::Learn(MoosePrefixAddress addr, Ptr<BridgePortNetDevice> port){

	Time now = Simulator::Now ();	

	Prefix &state = m_prefixState[addr];
	// Only update if the expiration is not in the future
	if(now + m_time > state.time){
		state.port = port;
		state.time = now + m_time;
	}
}

void MooseBridgeState::Learn(MooseSuffixAddress addr, Ptr<BridgePortNetDevice> port){
	
	Time now = Simulator::Now ();
	
	Port &state = m_portState[addr];
	// Only update if the expiration is not in the future
	if(now + m_time > state.time){
		state.port = port;
		state.time = now + m_time;
	}

}

void MooseBridgeState::Fixed(MoosePrefixAddress addr, Ptr<BridgePortNetDevice> port){
	

	Prefix &state = m_prefixState[addr];
	state.port = port;
	state.time = Simulator::GetMaximumSimulationTime();
}

Mac48Address MooseBridgeState::GetMac(MooseSuffixAddress addr){

	std::map<MooseSuffixAddress, Suffix*>::iterator iter = m_sufSuffixState.find(addr);
	
	if(iter == m_sufSuffixState.end()){

		// Not found.

		return Mac48Address::GetBroadcast();

	} else {
		
		Time now = Simulator::Now();     

		Suffix* state = iter->second;
	
		if(state->time > now){ 
			return iter->second->ethernet;
		} else {
			m_sufSuffixState.erase(iter);
			m_ethSuffixState.erase(iter->second->ethernet);
			return Mac48Address::GetBroadcast();
		}
	}

}

MooseSuffixAddress MooseBridgeState::GetSuffix(Mac48Address addr, MoosePrefixAddress prefix){

	Time now = Simulator::Now();

	std::map<Mac48Address, Suffix>::iterator iter = m_ethSuffixState.find(addr);

	if(iter == m_ethSuffixState.end()){
		// Allocate New Suffix
		MooseAddress moose = MooseAddress::Allocate(prefix); 
		Suffix &state = m_ethSuffixState[addr];
		state.ethernet = addr;
		state.suffix = moose.GetMooseSuffix();
		state.time = now + m_time;
		NS_LOG_LOGIC("Allocating New MOOSE Suffix: (" << moose.GetMoosePrefix().GetInt() << "," << moose.GetMooseSuffix().GetInt() << ") for " << addr);
		m_sufSuffixState[state.suffix] = &m_ethSuffixState[addr];


		return moose.GetMooseSuffix();

	} else {
		Suffix &state = iter->second;

		// Valid Entry

		if(state.time > now){
			MooseAddress moose = MooseAddress::Combine(prefix, state.suffix);
			NS_LOG_LOGIC("Using Allocated MOOSE Suffix: (" << prefix.GetInt() << "," << state.suffix.GetInt() << ")");

			if(now + m_time > state.time){
				state.time = now + m_time;
			}


			return moose.GetMooseSuffix();

		} else {
		
			// Expired Entry - Delete old and alloc new

			m_sufSuffixState.erase(state.suffix);
			m_ethSuffixState.erase(iter);

			// Allocate New

			Suffix &state = m_ethSuffixState[addr];
			MooseAddress moose = MooseAddress::Allocate(prefix);
			state.ethernet = addr;
			state.suffix = moose.GetMooseSuffix();
			state.time = now + m_time;

			NS_LOG_LOGIC("Expired MOOSE Suffix: (" << prefix.GetInt() << "," << state.suffix.GetInt() << ")");

			m_sufSuffixState[state.suffix] = &m_ethSuffixState[addr];


			return moose.GetMooseSuffix();

		}
	}
}

Ptr<BridgePortNetDevice> MooseBridgeState::GetPort(MoosePrefixAddress addr){

	Time now = Simulator::Now();

	std::map<MoosePrefixAddress, Prefix>::iterator iter = m_prefixState.find(addr);

	if(iter != m_prefixState.end() && iter->second.time > now){

		return iter->second.port;
	}

	return NULL;

}

Ptr<BridgePortNetDevice> MooseBridgeState::GetPort(MooseSuffixAddress addr){

	Time now = Simulator::Now();

	std::map<MooseSuffixAddress, Port>::iterator iter = m_portState.find(addr);

	if(iter != m_portState.end() && iter->second.time > now){

		return iter->second.port;
	}

	return NULL;

}

unsigned long MooseBridgeState::GetSize(){
	return m_prefixState.size() + m_portState.size();
}

}

