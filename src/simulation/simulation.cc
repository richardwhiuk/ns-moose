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

#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/core-module.h"
#include "ns3/helper-module.h"
#include "ns3/bridge-module.h"
#include "ns3/topology-module.h"

#include <iostream>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MooseSimulation");

void capture_state(std::ostream& file, LinkLayerHelper::Network& n){

	file << "ns-moose" << std::endl; 		// MAGIC
	file << 3 << std::endl;			// Type
	file << 1 << std::endl;			// Version

	std::map<long, NetDeviceContainer>::iterator i;
	NetDeviceContainer::Iterator j;

	Ptr<BridgeNetDevice> bridge;

	for(i = n.bridges.begin(); i != n.bridges.end(); ++i){
		for(j = i->second.Begin(); j != i->second.End(); ++j){
			bridge = (*j)->GetObject<BridgeNetDevice>();
			if(bridge != NULL){
				file << *bridge << std::endl;
			}
		}
	}
}

void setup(LinkLayerHelper::Network& n, Topology& t, std::istream& file){

	uint16_t port = 9;   // Discard port (RFC 863)

	std::string line;

	std::getline(file, line);
	if(line != "ns-moose"){
		throw new std::runtime_error("Invalid Network Data File");
	}

	unsigned long type;
	file >> type;
	if(type != 2){
		throw new std::runtime_error("Invalid Network Data File Type");
	}

	unsigned long version;
	file >> version;
	if(version != 1){
		throw new std::runtime_error("Invalid Network Data File Version");
	}

	std::map<unsigned long, bool> hosts;

	while(file.good() && !file.eof()){
		double time;
		unsigned long source;
		unsigned long destination;
		unsigned long packets;

		file >> time;
		file >> source;
		file >> destination;
		file >> packets;

		if(file.good()){

			if(source >= t.hosts){
				throw new std::runtime_error("Invalid Source in Network Data");
			}

			if(destination >= t.hosts){
				throw new std::runtime_error("Invalid Destination in Network Data");
			}

			if(packets == 0){
				throw new std::runtime_error("Invalid number of packets in Network Data.");
			}

			hosts[destination] = true;

			UdpClientHelper helper(n.interfaces[destination].GetAddress(0), port);
			helper.SetAttribute("MaxPackets", UintegerValue(packets));
			ApplicationContainer app = helper.Install(n.hostNodes.Get(source));
			app.Start (Seconds (time));

		}
	}

	NodeContainer serverNodes;
	std::map<unsigned long, bool>::iterator it;
	for(it = hosts.begin(); it != hosts.end(); it ++){
		serverNodes.Add(n.hostNodes.Get(it->first));
	}

	UdpServerHelper udpServerHelper (port);
	ApplicationContainer udpServers = udpServerHelper.Install (serverNodes);
	udpServers.Start (Seconds (0.0));

}


int main (int argc, char *argv[])
{

try {

	std::string csmaTraceFile, pcapTraceFile;
	std::string ipTraceFile;
	std::string networkFile;
	std::string dataFile;
	std::string stateFile;
	std::string linkLayer = "moose";
	std::string routing = "static";

	CommandLine cmd;			// Allow CommandLine args
	cmd.AddValue("link", "Link Layer? (moose|ethernet) [moose]", linkLayer);
	cmd.AddValue("csma", "CSMA Trace File", csmaTraceFile);
	cmd.AddValue("pcap", "CSMA PCAP Trace File", pcapTraceFile);
	cmd.AddValue("ip", "IPv4 Trace File", ipTraceFile);
	cmd.AddValue("network", "Network Topology File", networkFile);
	cmd.AddValue("data", "Network Data File", dataFile);
	cmd.AddValue("state", "Bridge State File", stateFile);
	cmd.AddValue("routing", "Dynamic Routing? (static|dynamic) [static]", routing);
	cmd.Parse (argc, argv);

	std::ifstream networkStream(networkFile.c_str(), std::ifstream::in);

	if(networkStream.fail()){
		std::ostringstream ss;
		ss << "Failed to open network data file [" << networkFile << "]";
		throw new std::runtime_error(ss.str());
	}

	NS_LOG_INFO ("Create Topology");

	Topology t(networkStream);

	// Link Layer Helper

	LinkLayerHelper link;

	if(link.SupportsLinkLayer(linkLayer)){
		link.SetLinkLayer(linkLayer);
	} else {
		std::ostringstream ss;
		ss << "Unsupported Link Layer Protocol [" << linkLayer << "]";
		throw new std::runtime_error(ss.str());
	}

	if(routing == "dynamic"){
		link.EnableDynamicRouting();
	} else if(routing == "static"){
		link.DisableDynamicRouting();
	} else {
		std::ostringstream ss;
		ss << "Unsupported Routing Type [" << routing << "]";
		throw new std::runtime_error(routing);
	}

	NS_LOG_INFO ("Create Network");

	// Setup Network

	LinkLayerHelper::Network n = link.Create(t);
	
	// Create UDP Applications to send the two packets

	NS_LOG_INFO ("Create Application");

	std::ifstream dataStream(dataFile.c_str(), std::ifstream::in);

	if(dataStream.fail()){
		std::ostringstream ss;
		ss << "Failed to open network data file [" << dataFile << "]";
		throw new std::runtime_error(ss.str());

	}

	setup(n, t, dataStream);

	// Tracing

	NS_LOG_INFO ("Configure Tracing");

	if(csmaTraceFile != "" || ipTraceFile != ""){
		AsciiTraceHelper ascii;

		if(csmaTraceFile != ""){
			Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream (csmaTraceFile);
			link.csma.EnableAsciiAll(stream);
		}

		if (ipTraceFile != ""){
			Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream (ipTraceFile);
			link.internet.EnableAsciiIpv4All(stream);
		}
	}

	if(pcapTraceFile != ""){
		link.csma.EnablePcapAll(pcapTraceFile, true);
	}


	NS_LOG_INFO ("Run Simulation");
	Simulator::Run ();

	if(stateFile != ""){
		std::ofstream stateStream(stateFile.c_str(),  std::ios_base::out | std::ios_base::trunc);
		capture_state(stateStream, n);
	}

	NS_LOG_INFO ("Destroy Simulation");
	Simulator::Destroy ();

	NS_LOG_INFO ("Done");


} catch(std::runtime_error* e) {
	std::cerr << e->what() << std::endl;
	return 1;
}

return 0;

}

