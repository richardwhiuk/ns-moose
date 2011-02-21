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

// Topology
//
//       H0                 H1
//       |		    |
//  ------------      ------------
//  | Switch 0 |------| Switch 1 |
//  ------------      ------------
//       |                  |
//  ------------      ------------
//  | Switch 2 |------| Switch 3 |
//  ------------      ------------
//       |		    |
//       H2		    H3
//
// - 100 Mb/s
// - CBR/UDP flows from Host (h) 0 to Host 1 and then from Host 0 to Host 2
// - DropTail queues

#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/core-module.h"
#include "ns3/helper-module.h"
#include "ns3/bridge-module.h"

#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MooseSimulation");

int main (int argc, char *argv[])
{

  bool useMoose = true;
  std::string tracefile = "simulation.tr";

  CommandLine cmd;			// Allow CommandLine args
  cmd.AddValue("moose", "Use MOOSE instead of Ethernet? [true]", useMoose);
  cmd.AddValue("trace", "Trace file output [simulation.tr]", tracefile);
  cmd.Parse (argc, argv);

  // Moose Helper

  MooseHelper moose;

  if(useMoose){
  	moose.SetMoose();
  } else {
	moose.SetEthernet();
  }

  // Setup Network

  MooseHelper::Network n;

  NS_LOG_INFO ("Configure Topology.");

  n.t.bridges = 4;
  n.t.hosts = 4;

  // Link hosts to bridges

  n.t.hostLinks[0] = 0;
  n.t.hostLinks[1] = 1;
  n.t.hostLinks[2] = 2; 
  n.t.hostLinks[3] = 3;

  // Link the two bridges together
 
  n.t.bridgeLinks.insert(std::make_pair<long,long>(0,1));
  n.t.bridgeLinks.insert(std::make_pair<long,long>(0,2));
  n.t.bridgeLinks.insert(std::make_pair<long,long>(1,3));
  n.t.bridgeLinks.insert(std::make_pair<long,long>(2,3));

  // Create Network

  moose.Create(n);


  //
  // Create UDP Applications to send the two packets
  //

  NS_LOG_INFO ("Create Applications.");

  uint16_t port = 9;   // Discard port (RFC 863)

  int i,j;

  // 0->1, 0->2, 0->3, 1->0, 1->2, 1->3, 2->0, 2->1, 2->3, 3->0, 3->1, 3->2

  NodeContainer serverNodes;

  for(i = 0; i < 4; ++i){

    for(j = 0; j < 4; ++j){

	if(i != j){		

  	   UdpClientHelper udpClientHelper (n.interfaces[j].GetAddress(0), port);		// i->j
  	   udpClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));

  	   ApplicationContainer udpClient = udpClientHelper.Install (n.hosts.Get(i));

           int start = i*4 + j; 

  	   udpClient.Start (Seconds (start));
  	   udpClient.Stop  (Seconds (start + 1));
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
  moose.csma.EnableAsciiAll (ascii.CreateFileStream (tracefile));

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

}

