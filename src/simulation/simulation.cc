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
#include <cstdlib>
#include <ctime>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MooseSimulation");

int main (int argc, char *argv[])
{

  bool useMoose = true;
  std::string tracefile = "simulation.tr";
  long hosts = 1;
  long size = 2;
  std::string type = "mesh";
  long seed = 0;
  long link = 2;

  seed = std::time(NULL);

  CommandLine cmd;			// Allow CommandLine args
  cmd.AddValue("moose", "Use MOOSE instead of Ethernet? [true]", useMoose);
  cmd.AddValue("trace", "Trace file output [simulation.tr]", tracefile);
  cmd.AddValue("type", "Network type? [mesh]", type);
  cmd.AddValue("size", "Network size [2]", size);
  cmd.AddValue("hosts", "Number of hosts per switch [1]", hosts);
  cmd.AddValue("seed", "Random seed" , seed);
  cmd.AddValue("link", "Probability of link 1/[2]" , link);
  cmd.Parse (argc, argv);

  std::srand(seed);

  Topology t;

  if(type == "mesh"){
      
	t = MeshTopologyHelper::Create(hosts, size);

  } else {

	std::cerr << "Unknown Network Type [" << type << "]" << std::endl;
	return 1;	

  }

  // Moose Helper

  MooseHelper moose;

  if(useMoose){
  	moose.SetMoose();
  } else {
	moose.SetEthernet();
  }

  // Setup Network

  MooseHelper::Network n = moose.Create(t);

  //
  // Create UDP Applications to send the two packets
  //

  NS_LOG_INFO ("Create Applications.");

  uint16_t port = 9;   // Discard port (RFC 863)

  NodeContainer serverNodes;

  long i, j;

  for(i = 0; i < t.hosts; ++i){

    for(j = 0; j < t.hosts; ++j){

	if(i != j){		

	   //if((rand() % link) == 0){

	  	   UdpClientHelper udpClientHelper (n.interfaces[i].GetAddress(0), port);		// j->i
  		   udpClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));

	  	   ApplicationContainer udpClientA = udpClientHelper.Install (n.hosts.Get(j));

	  	   udpClientA.Start (Seconds ((i*t.hosts) + j));
  		   udpClientA.Stop  (Seconds ((i*t.hosts) + j + 1));

	  // }
	}
     }

     serverNodes.Add(n.hosts.Get(i)); 		// Add host i as a server.

  }

  UdpServerHelper udpServerHelper (port);
  ApplicationContainer udpServers = udpServerHelper.Install (serverNodes);
  udpServers.Start (Seconds (0.0));

  // Tracing

  NS_LOG_INFO ("Configure Tracing.");

  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream (tracefile);
  moose.csma.EnableAsciiAll(stream);
 // moose.internet.EnableAsciiIpv4All(stream);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  return 0;

}

