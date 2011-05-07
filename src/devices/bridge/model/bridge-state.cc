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

#include "bridge-state.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("BridgeState");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (BridgeState);

TypeId BridgeState::GetTypeId (void){

static TypeId tid = TypeId ("ns3::BridgeState")
.SetParent<Object> ()
.AddConstructor<BridgeState> ()
.AddAttribute ("ExpirationTime",
"Time it takes for learned MAC state entry to expire.",
TimeValue (Seconds (300)),
MakeTimeAccessor (&BridgeState::m_time),
MakeTimeChecker ())
.AddAttribute ("StateSize", "The size of the State Table",
UintegerValue (8000),
MakeUintegerAccessor (&BridgeState::m_max),
MakeUintegerChecker<unsigned long> ())
;
return tid;
}

unsigned long BridgeState::GetMaxSize() const {
	return m_max;
}

BridgeState::BridgeState(){

}

BridgeState::~BridgeState(){

}

void BridgeState::Learn(ns3::Mac48Address addr, Ptr<BridgePortNetDevice> port){
	std::map<Mac48Address, Host>::iterator it = m_learnState.find(addr);
	if(it != m_learnState.end() || m_learnState.size() < m_max){
	      Host &state = m_learnState[addr];
	      state.associatedPort = port;
	      state.expirationTime = Simulator::Now () + m_time;
	}
}

void BridgeState::SetMaxSize(unsigned long max){
	m_max = max;
}

Ptr<BridgePortNetDevice> BridgeState::GetPort(ns3::Mac48Address addr){
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

unsigned long BridgeState::GetSize(){
	return m_learnState.size();
}

void BridgeState::SetExpirationTime(Time time){
	m_time = time;
}

Time BridgeState::GetExpirationTime(){
	return m_time;
}

}


