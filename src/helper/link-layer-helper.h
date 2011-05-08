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

#include "ns3/topology.h"
#include "ns3/node.h"
#include "ns3/moose-bridge-net-device.h"
#include "ns3/moose-bridge-helper.h"
#include "ns3/ethernet-bridge-helper.h"
#include "ns3/ethernet-bridge-net-device.h"
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
class LinkLayerHelper {

public:

	typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, long> > graph_t;

	typedef boost::graph_traits< graph_t >::vertex_descriptor vertex_descriptor;
	typedef boost::graph_traits< graph_t >::edge_descriptor edge_descriptor;
	
	struct Network {
		
		// Contains all the nodes :-)

		NodeContainer bridgeNodes;
		NodeContainer hostNodes;

		// CSMA NetDevices

		std::map<long, NetDeviceContainer> bridgeDevices;
		std::map<long, NetDeviceContainer> hostDevices;

		// Bridges

		std::map<long, NetDeviceContainer> bridges;

		// IP Interfaces

		std::map<long, Ipv4InterfaceContainer > interfaces;

	};

	CsmaHelper csma;
	Ipv4AddressHelper ipv4;
	InternetStackHelper internet;
	EthernetBridgeHelper ethernetHelper;
	MooseBridgeHelper mooseHelper;

	LinkLayerHelper();

	~LinkLayerHelper();

	bool SupportsLinkLayer(std::string linkLayer);

	void SetLinkLayer(std::string linkLayer);

	void EnableDynamicRouting();

	void DisableDynamicRouting();

	Network Create(Topology& t);

private:

	bool moose;
	bool routing;


};

}

#endif

