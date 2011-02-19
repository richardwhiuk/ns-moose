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

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include "moose-helper.h"
#include "ns3/moose-prefix-address.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("MooseHelper");

namespace ns3 {

MooseHelper::MooseHelper(){
	// Sensible defaults.

	// Medium

	csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
	csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

	// Internet

	// IPv4

	ipv4.SetBase ("10.0.0.0", "255.0.0.0");

	// Ethernet Technology

	moose = true;
	routing = true; 	// Simulation based routing
}

MooseHelper::~MooseHelper(){

}

void MooseHelper::SetMoose(){
	moose = true;
}

void MooseHelper::SetEthernet(){
	moose = false;
}

void MooseHelper::EnableDynamicRouting(){
	routing = false;
}	

void MooseHelper::DisableDynamicRouting(){
	routing = true;
}

void MooseHelper::Create(MooseHelper::Network& n){

	// Create the bridges and hosts

	n.bridges.Create(n.t.bridges);
	n.hosts.Create(n.t.hosts);

	std::map<long, std::map<long, Ptr<NetDevice> > > portMap;

	// Link them up

	// Host-Bridge

	for(
		Topology::HostLinks::iterator it = n.t.hostLinks.begin(); 
		it != n.t.hostLinks.end(); 
		it ++){

		NodeContainer nc;
		
		if(it->first >= n.t.hosts){
			assert(false);
		}

		if(it->second >= n.t.bridges){
			assert(false);
		}

		nc.Add(n.hosts.Get(it->first));
		nc.Add(n.bridges.Get(it->second));

		NetDeviceContainer link = csma.Install(nc);

		n.hostDevices[it->first].Add(link.Get(0));
		n.bridgeDevices[it->second].Add(link.Get(1));
	}

	// Bridge-Bridge 

	for(
		Topology::BridgeLinks::iterator it = n.t.bridgeLinks.begin(); 
		it != n.t.bridgeLinks.end(); 
		it ++){

		NodeContainer nc;
		
		if(it->first >= n.t.bridges){
			assert(false);
		}

		if(it->second >= n.t.bridges){
			assert(false);
		}

		nc.Add(n.bridges.Get(it->first));
		nc.Add(n.bridges.Get(it->second));

		NetDeviceContainer link = csma.Install(nc);

		n.bridgeDevices[it->first].Add(link.Get(0));
		n.bridgeDevices[it->second].Add(link.Get(1));

		portMap[it->first][it->second] = link.Get(0);
		portMap[it->second][it->first] = link.Get(1);

	}


	// Install Bridges

	if(moose){
	
		// Routing

		if(routing){

			// Djkstra - Give all links the same weight for now.

			std::vector<long> weights(n.t.bridgeLinks.size(), 1);

			const long num_nodes = n.t.bridges;

			graph_t g(n.t.bridgeLinks.begin(), n.t.bridgeLinks.end(), weights.begin(), num_nodes);
			boost::property_map<graph_t, boost::edge_weight_t>::type weightmap = get(boost::edge_weight, g);

			for(long root = 0; root < n.t.bridges; root ++){

				std::vector<vertex_descriptor> p(num_vertices(g));
				vertex_descriptor s = boost::vertex(root,g);
				boost::dijkstra_shortest_paths(g, s, boost::predecessor_map(&p[0]));

				std::cout << "parents from " << root << ":" << std::endl;
				boost::graph_traits<graph_t>::vertex_iterator vi, vend;

				std::map<MoosePrefixAddress, Ptr<NetDevice> > routes;

				for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
					std::cout << root << "->" << *vi << " ";
	
					if(*vi == s){
						// Don't inject into routing table.
						std::cout << "At destination" << std::endl;
					} else {
						vertex_descriptor current = *vi;
						while(p[current] != p[p[current]]){
							current = p[current];
						}
				
						if(p[current] == s){
							// Inject routing decision
	
							std::cout << current << std::endl;

							routes[MoosePrefixAddress(root)] = portMap[root][current];
	
						} else {

							// Don't inject

							std::cout << "unreachable" << std::endl;
						}
					}
				}
				std::cout << std::endl;

				// Create the switch

				Ptr<Node> bridgeNode = n.bridges.Get(root);
				mooseHelper.Install(bridgeNode, n.bridgeDevices[root], MoosePrefixAddress(root), routes);

			}

		} else {
			
			// Realtime routing

			for(long i = 0; i < n.t.bridges; i ++){
				Ptr<Node> bridgeNode = n.bridges.Get(i);
				mooseHelper.Install(bridgeNode, n.bridgeDevices[i]);
			}
		}
	} else {
		for(long i = 0; i < n.t.bridges; i ++){
			Ptr<Node> bridgeNode = n.bridges.Get(i);
			ethernetHelper.Install(bridgeNode, n.bridgeDevices[i]);
		}
	}

	// Setup the internet

	internet.Install(n.hosts);

	for(long i = 0; i < n.t.hosts; i ++){
		ipv4.Assign(n.hostDevices[i]);
	}

}


}

