/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008,2009 IITP RAS
 *
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

#ifndef MOOSE_HELPER_H
#define MOOSE_HELPER_H

#include "ns3/node.h"
#include "ns3/moose-bridge-net-device.h"
#include "ns3/moose-bridge-helper.h"
#include "ns3/bridge-helper.h"
#include "internet-stack-helper.h"
#include "ipv4-address-helper.h"
#include "csma-helper.h"

// Boost Graph Library for static route computation.

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <map>

namespace ns3 {

/** 
 * \ingroup bridge
 * 
 * \brief Helper to create Ethernet / MOOSE bridged networks
 */
class MooseHelper {

public:

	typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int> > graph_t;

	typedef boost::graph_traits< graph_t >::vertex_descriptor vertex_descriptor;
	typedef boost::graph_traits< graph_t >::edge_descriptor edge_descriptor;
	
	struct Topology {
	
	public:
		long bridges;
		long hosts;
		typedef std::map<long,long> HostLinks;
		typedef std::pair<long,long> BridgeLink;

		struct BridgeLinkCompare {
			// we don't care about ordering inside a pair, but we do care about pair order

			bool operator()( BridgeLink const &lhs, BridgeLink const &rhs) {
				if(lhs.first < lhs.first){
					if(rhs.first < rhs.second){
						return (lhs.first < rhs.first) ? true : (lhs.second < rhs.second);
					} else {
						return (lhs.first < rhs.second) ? true : (lhs.second < rhs.first);
					}
				} else {
					if(rhs.first < rhs.second){
						return (lhs.second < rhs.first) ? true : (lhs.first < rhs.second);
					} else {
						return (lhs.second < rhs.second) ? true : (lhs.first < rhs.first);
					}
				}
			}
		};
	
		typedef std::set<BridgeLink, BridgeLinkCompare> BridgeLinks;

		HostLinks hostLinks;
		BridgeLinks bridgeLinks;

	};

	struct Network {
		
		Topology t;

		// Contains all the nodes :-)

		NodeContainer bridges;
		NodeContainer hosts;

		// NetDevices

		std::map<long, NetDeviceContainer> bridgeDevices;
		std::map<long, NetDeviceContainer> hostDevices;
	};

	CsmaHelper csma;
	Ipv4AddressHelper ipv4;
	InternetStackHelper internet;
	MooseBridgeHelper mooseHelper;
	BridgeHelper ethernetHelper;

	MooseHelper();

	~MooseHelper();

	void SetMoose();

	void SetEthernet();

	void EnableDynamicRouting();

	void DisableDynamicRouting();

	void Create(Network& t);

private:

	bool moose;
	bool routing;


};

}

#endif

