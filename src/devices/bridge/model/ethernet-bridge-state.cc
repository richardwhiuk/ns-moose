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

#include "ethernet-bridge-state.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("EthernetBridgeState");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (EthernetBridgeState);

TypeId EthernetBridgeState::GetTypeId (void){

static TypeId tid = TypeId ("ns3::EthernetBridgeState")
.SetParent<Object> ()
.AddConstructor<EthernetBridgeState> ()
.AddAttribute ("ExpirationTime",
"Time it takes for learned MAC state entry to expire.",
TimeValue (Seconds (300)),
MakeTimeAccessor (&EthernetBridgeState::m_time),
MakeTimeChecker ())
.AddAttribute ("StateSize", "The size of the State Table",
UintegerValue (8000),
MakeUintegerAccessor (&EthernetBridgeState::m_max),
MakeUintegerChecker<unsigned long> ())
;
return tid;
}

unsigned long EthernetBridgeState::GetMaxSize() const {
	return m_max;
}

EthernetBridgeState::EthernetBridgeState(){

}

EthernetBridgeState::~EthernetBridgeState(){

}

void EthernetBridgeState::Learn(ns3::Mac48Address addr, Ptr<BridgePortNetDevice> port){
	std::map<Mac48Address, Host>::iterator it = m_learnState.find(addr);
	if(it != m_learnState.end() || m_learnState.size() < m_max){
	      Host &state = m_learnState[addr];
	      state.associatedPort = port;
	      state.expirationTime = Simulator::Now () + m_time;
	}
}

void EthernetBridgeState::SetMaxSize(unsigned long max){
	m_max = max;
}

Ptr<BridgePortNetDevice> EthernetBridgeState::GetPort(ns3::Mac48Address addr){
	Time now = Simulator::Now ();
	std::map<Mac48Address, Host>::iterator iter = m_learnState.find(addr);
	if (iter != m_learnState.end()){
		Host &state = iter->second;
		if (state.expirationTime > now){
			return state.associatedPort;
		} else {
			m_learnState.erase (iter);
		}
	}
}

unsigned long EthernetBridgeState::GetSize(){
	return m_learnState.size();
}

void EthernetBridgeState::SetExpirationTime(Time time){
	m_time = time;
}

Time EthernetBridgeState::GetExpirationTime(){
	return m_time;
}

std::ostream& operator<<(std::ostream& file, EthernetBridgeState& state){
	// Output state
	file << state.m_learnState.size();
	std::map<Mac48Address, EthernetBridgeState::Host>::iterator it;
	for(it = state.m_learnState.begin(); it != state.m_learnState.end(); ++it){
		file << std::endl;
		file << it->first << std::endl;
		file << it->second.associatedPort << std::endl;
		file << it->second.expirationTime;
	}
	return file;
}

}


