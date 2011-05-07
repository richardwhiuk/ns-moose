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

#ifndef MOOSE_BRIDGE_STATE_H
#define MOOSE_BRIDGE_STATE_H

#include "bridge-port-net-device.h"
#include "ns3/object-factory.h"
#include "ns3/mac48-address.h"
#include "ns3/nstime.h"
#include "ns3/moose-address.h"
#include "ns3/moose-prefix-address.h"
#include "ns3/moose-suffix-address.h"

namespace ns3 {

class MooseBridgeState : public Object {

public:

	static TypeId GetTypeId (void);
	MooseBridgeState();
	~MooseBridgeState();

	unsigned long GetSize();

	unsigned long GetMaxSize() const;
	bool SetMaxSize(const unsigned long max);

	void Learn(MooseSuffixAddress suffix, Ptr<BridgePortNetDevice> port);
	void Learn(MoosePrefixAddress prefix, Ptr<BridgePortNetDevice> port);

	void Fixed(MoosePrefixAddress prefix, Ptr<BridgePortNetDevice> port);

	MooseSuffixAddress GetSuffix(Mac48Address addr, MoosePrefixAddress prefix);

	Ptr<BridgePortNetDevice> GetPort(MoosePrefixAddress addr);
	Ptr<BridgePortNetDevice> GetPort(MooseSuffixAddress addr);

	Mac48Address GetMac(MooseSuffixAddress addr);

private:

	unsigned long m_max;

	// Remote Moose State

	struct Prefix {
		Ptr<BridgePortNetDevice> port;
		Time time;
	};  

	// Local Moose State

	struct Suffix {
		Mac48Address ethernet;
		MooseSuffixAddress suffix;
		Time time;
	};

	struct Port {
		Ptr<BridgePortNetDevice> port;
		Time time;
	};

	Time m_time;

	std::map<MoosePrefixAddress, Prefix> m_prefixState;
	std::map<Mac48Address, Suffix> m_ethSuffixState;
	std::map<MooseSuffixAddress, Suffix*> m_sufSuffixState;
	std::map<MooseSuffixAddress, Port> m_portState;


};

}

#endif

