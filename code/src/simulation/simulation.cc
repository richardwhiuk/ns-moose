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
//   H0       H1
//   |        |
//  ------------      ------------
//  | Switch 0 |------| Switch 2 |
//  ------------      ------------
//       |                  |
//  ------------      ------------
//  | Switch 1 |------| Switch 3 |
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

  CommandLine cmd;			// Allow CommandLine args
  cmd.Parse (argc, argv);

  // Moose Helper

  MooseHelper moose;

  // Setup Network

  MooseHelper::Network n;

  NS_LOG_INFO ("Configure Topology.");

  n.t.bridges = 4;
  n.t.hosts = 4;

  // Link hosts to bridges

  n.t.hostLinks[0] = 0;
  n.t.hostLinks[1] = 0;
  n.t.hostLinks[2] = 1; 
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

  UdpClientHelper udpClientHelperA (n.interfaces[1].GetAddress(0), port);		// Target: Host 1
  udpClientHelperA.SetAttribute ("MaxPackets", UintegerValue (1));

  ApplicationContainer udpClientA = udpClientHelperA.Install (n.hosts.Get(0));		// n0 -> n1
  udpClientA.Start (Seconds (1.0));
  udpClientA.Stop  (Seconds (3.0));

  UdpClientHelper udpClientHelperB (n.interfaces[2].GetAddress(0), port);		// Target: Host 2
  udpClientHelperB.SetAttribute ("MaxPackets", UintegerValue (1));

  ApplicationContainer udpClientB = udpClientHelperB.Install (n.hosts.Get(0));		// n0 -> n2
  udpClientB.Start (Seconds (6.0));
  udpClientB.Stop  (Seconds (8.0));

  NodeContainer serverNodes;
  serverNodes.Add(n.hosts.Get(1)); // Server: Host 1
  serverNodes.Add(n.hosts.Get(2)); // Server: Host 2

  UdpServerHelper udpServerHelper (port);
  ApplicationContainer udpServers = udpServerHelper.Install (serverNodes);
  udpServers.Start (Seconds (0.0));

  // Tracing

  NS_LOG_INFO ("Configure Tracing.");

  AsciiTraceHelper ascii;
  moose.csma.EnableAsciiAll (ascii.CreateFileStream ("moose-bridge.tr"));

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

}

