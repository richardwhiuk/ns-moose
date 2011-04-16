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

			if(source > t.hosts){
				throw new std::runtime_error("Invalid Source in Network Data");
			}

			if(destination > t.hosts){
				throw new std::runtime_error("Invalid Destination in Network Data");
			}

			if(packets == 0){
				throw new std::runtime_error("Invalid number of packets in Network Data.");
			}

			hosts[destination] = true;

			UdpClientHelper helper(n.interfaces[destination].GetAddress(0), port);
			helper.SetAttribute("MaxPackets", UintegerValue(packets));
			ApplicationContainer app = helper.Install(n.hosts.Get(source));
			app.Start (Seconds (time));

		}
	}

	NodeContainer serverNodes;
	std::map<unsigned long, bool>::iterator it;
	for(it = hosts.begin(); it != hosts.end(); it ++){
		serverNodes.Add(n.hosts.Get(it->first));
	}

	UdpServerHelper udpServerHelper (port);
	ApplicationContainer udpServers = udpServerHelper.Install (serverNodes);
	udpServers.Start (Seconds (0.0));

}


int main (int argc, char *argv[])
{

try {

	std::string csmaTraceFile;
	std::string ipTraceFile;
	std::string networkFile;
	std::string dataFile;
	std::string linkLayer = "moose";

	CommandLine cmd;			// Allow CommandLine args
	cmd.AddValue("link", "Link Layer? (moose|ethernet) [moose]", linkLayer);
	cmd.AddValue("csma", "CSMA Trace File", csmaTraceFile);
	cmd.AddValue("ip", "IPv4 Trace File", ipTraceFile);
	cmd.AddValue("network", "Network Topology File", networkFile);
	cmd.AddValue("data", "Network Data File", dataFile);
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

	if(csmaTraceFile != "" && ipTraceFile != ""){
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


	NS_LOG_INFO ("Run Simulation");
	Simulator::Run ();

	NS_LOG_INFO ("Destroy Simulation");
	Simulator::Destroy ();

	NS_LOG_INFO ("Done");


} catch(std::runtime_error* e) {
	std::cerr << e->what() << std::endl;
	return 1;
}

return 0;

}

