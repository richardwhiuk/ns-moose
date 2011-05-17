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
#include "link-layer-helper.h"
#include "ns3/moose-prefix-address.h"
#include "ns3/log.h"

#include <vector>
#include <algorithm>
#include <queue>

NS_LOG_COMPONENT_DEFINE ("LinkLayerHelper");

namespace ns3 {

LinkLayerHelper::LinkLayerHelper(){
	// Sensible defaults.

	// Medium

	csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate("1Gbps")));
	csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (20)));

	// Internet

	// IPv4

	ipv4.SetBase ("10.0.0.0", "255.0.0.0");

	// Ethernet Technology

	moose = true;
	routing = true; 	// Simulation based routing
}

LinkLayerHelper::~LinkLayerHelper(){

}

bool LinkLayerHelper::SupportsLinkLayer(std::string linkLayer){
	if(linkLayer == "moose" || linkLayer == "ethernet"){
		return true;
	}
	return false;
}

void LinkLayerHelper::SetLinkLayer(std::string linkLayer){
	if(linkLayer == "moose"){
		moose = true;
	} else if(linkLayer == "ethernet"){
		moose = false;
	}
}

void LinkLayerHelper::EnableDynamicRouting(){
	routing = false;
}	

void LinkLayerHelper::DisableDynamicRouting(){
	routing = true;
}

LinkLayerHelper::Network LinkLayerHelper::Create(Topology& t){

	// Create the bridges and hosts

	Network n;

	n.bridgeNodes.Create(t.bridges);
	n.hostNodes.Create(t.hosts);

	std::map<long, std::map<long, Ptr<NetDevice> > > portMap;

	// Link them up

	// Host-Bridge

	for(
		Topology::HostLinks::iterator it = t.hostLinks.begin(); 
		it != t.hostLinks.end(); 
		it ++){

		NodeContainer nc;
		
		if(it->first >= t.hosts){
			assert(false);
		}

		if(it->second >= t.bridges){
			assert(false);
		}

		nc.Add(n.hostNodes.Get(it->first));
		nc.Add(n.bridgeNodes.Get(it->second));

		NetDeviceContainer link = csma.Install(nc);

		n.hostDevices[it->first].Add(link.Get(0));
		n.bridgeDevices[it->second].Add(link.Get(1));
	}

	// Bridge-Bridge 

	for(
		Topology::BridgeLinks::iterator it = t.bridgeLinks.begin(); 
		it != t.bridgeLinks.end(); 
		it ++){

		NodeContainer nc;
		
		if(it->first >= t.bridges){
			assert(false);
		}

		if(it->second >= t.bridges){
			assert(false);
		}

		nc.Add(n.bridgeNodes.Get(it->first));
		nc.Add(n.bridgeNodes.Get(it->second));

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

			std::vector<long> weights(t.bridgeLinks.size(), 1);

			const long num_nodes = t.bridges;

			graph_t g(t.bridgeLinks.begin(), t.bridgeLinks.end(), weights.begin(), num_nodes);
			boost::property_map<graph_t, boost::edge_weight_t>::type weightmap = get(boost::edge_weight, g);

			for(long root = 0; root < t.bridges; root ++){

				std::vector<vertex_descriptor> p(num_vertices(g));
				vertex_descriptor s = boost::vertex(root,g);
				boost::dijkstra_shortest_paths(g, s, boost::predecessor_map(&p[0]));

				boost::graph_traits<graph_t>::vertex_iterator vi, vend;

				std::multimap<Ptr<NetDevice>, MoosePrefixAddress> routes;

				for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
	
					if(*vi == s){
						// Don't inject into routing table.
					} else {
						vertex_descriptor current = *vi;
						while(p[current] != p[p[current]]){
							current = p[current];
						}
				
						if(p[current] == s){
							// Inject routing decision
	
							routes.insert(std::make_pair(portMap[root][current],MoosePrefixAddress(*vi)));
	
						} else {
							// Don't inject
						}
					}
				}

				// Create the switch

				Ptr<Node> bridgeNode = n.bridgeNodes.Get(root);
				n.bridges[root] = mooseHelper.Install(bridgeNode, n.bridgeDevices[root], MoosePrefixAddress(root), routes);

			}

		} else {

			// Realtime routing

			for(long i = 0; i < t.bridges; i ++){
				Ptr<Node> bridgeNode = n.bridgeNodes.Get(i);
				n.bridges[i] = mooseHelper.Install(bridgeNode, n.bridgeDevices[i]);
			}
		}
	} else {
		if(routing){

			// STP doesn't do a Minimum Spanning Tree, it does a minimum tree based on the root node.

			// Since all links are 1, we can do this in a very simple manner

			// We assume a connected network - possibly incorrectly.

			Topology::BridgeLinks links(t.bridgeLinks);
			std::map<unsigned long, std::map<unsigned long, bool> > spanning;

			std::queue<unsigned long> toCheck; // Nodes to check
			std::map<unsigned long, bool> inTree; // In the tree?

			for(unsigned long i = 0; i < t.bridges; ++i){
				inTree[i] = false;
				for(unsigned long j = 0; j < t.bridges; ++j){
					spanning[i][j] = false;
				}
			}

			bool finished = false;

			unsigned long node = 0;
			unsigned long number = 1;

			while(!finished){
				Topology::BridgeLinks::iterator it = links.begin();
				while(it != links.end()){

					if(it->first == node){
						if(!inTree[it->second]){
							spanning[node][it->second] = true;
							spanning[it->second][node] = true;
							inTree[it->second] = true;
							toCheck.push(it->second);
							number++;
						}
						Topology::BridgeLinks::iterator er = it;
						++it;
						links.erase(er);
					} else if(it->second == node){
						if(!inTree[it->first]){
							spanning[node][it->first] = true;
							spanning[it->first][node] = true;
							inTree[it->second] = true;
							toCheck.push(it->first);
							number ++;
						}
						Topology::BridgeLinks::iterator er = it;
						++it;
						links.erase(er);
					} else {
						++it;
					}
				}
				if(number == t.bridges){
					finished = true;
				} else if(toCheck.size() == 0){
					finished = true;
				} else {
					node = toCheck.front();
					toCheck.pop();
				}
			}

			// Disable the other ports not included.

			// bridge[0] has link to bridge[1] on port x => portMap[0][1] = x

			// Question: is bridge[0][1] in spanning tree? -> i.e. is there some x, for which tree[x] = <0,1>


			for(long i = 0; i < t.bridges; i ++){

				std::map<Ptr<NetDevice>, bool> portsEnabled;

				for(std::map<long, Ptr<NetDevice> >::iterator it = portMap[i].begin(); it != portMap[i].end(); it ++){
					portsEnabled[it->second] = spanning[i][it->first];
				}


				Ptr<Node> bridgeNode = n.bridgeNodes.Get(i);
				n.bridges[i] = ethernetHelper.Install(bridgeNode, n.bridgeDevices[i], portsEnabled);
			}
			
		
		} else {
			// Requires STP implementation

			for(long i = 0; i < t.bridges; i ++){

				Ptr<Node> bridgeNode = n.bridgeNodes.Get(i);
				n.bridges[i] = ethernetHelper.Install(bridgeNode, n.bridgeDevices[i]);
			}
		}
	}

	// Setup the internet

	internet.Install(n.hostNodes);

	for(long i = 0; i < t.hosts; i ++){
		n.interfaces[i] = ipv4.Assign(n.hostDevices[i]);
	}

	return n;

}


}

