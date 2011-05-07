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
#ifndef BRIDGE_STATE_H
#define BRIDGE_STATE_H

#include "bridge-port-net-device.h"
#include "ns3/object-factory.h"
#include "ns3/mac48-address.h"
#include "ns3/nstime.h"

namespace ns3 {

class BridgeState : public Object {

public:

	static TypeId GetTypeId (void);
	BridgeState ();
	virtual ~BridgeState ();

	unsigned long GetSize();

	unsigned long GetMaxSize() const;
	void SetMaxSize(const unsigned long max);

	void Learn(Mac48Address addr, Ptr<BridgePortNetDevice> port);
	Ptr<BridgePortNetDevice> GetPort(Mac48Address addr);

	void SetExpirationTime(Time time);
	
	Time GetExpirationTime();

private:

	unsigned long m_max;
	Time m_time;

	struct Host {
		Ptr<BridgePortNetDevice> associatedPort;
		Time expirationTime;
	};

	std::map<Mac48Address, Host> m_learnState;

};

}

#endif

