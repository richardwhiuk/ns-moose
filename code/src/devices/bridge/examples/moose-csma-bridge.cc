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
//   n0       n1
//   |        |
//  ------------
//  | Switch A |
//  ------------
//       |
//  ------------
//  | Switch B |
//  ------------
//       |
//       n2
//
// - 100 Mb/s
// - CBR/UDP flows from n0 to n1 and then from n0 to n2
// - DropTail queues 
//
// For comparison with moose-bridge

#include <iostream>
#include <fstream>

#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/core-module.h"
#include "ns3/helper-module.h"
#include "ns3/bridge-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MooseCsmaBridgeExample");

int main (int argc, char *argv[])
{

  CommandLine cmd;			// Allow CommandLine args
  cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer terminalsA, terminalsB;
  terminalsA.Create (2);		// n0,n1
  terminalsB.Create (1);		// n2

  NodeContainer switchA, switchB;
  switchA.Create (1);			// Switch A
  switchB.Create (1);			// Switch B

  NS_LOG_INFO ("Build Topology");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  NetDeviceContainer terminalDevices;
  NetDeviceContainer switchADevices, switchBDevices;
	
  // Link from n0,n1 <-> A

  for (int i = 0; i < 2; i++){
     NetDeviceContainer link = csma.Install (NodeContainer (terminalsA.Get(i), switchA));
     terminalDevices.Add (link.Get (0));
     switchADevices.Add (link.Get (1));
  }

  // Link from A <-> B

  {
     NetDeviceContainer link = csma.Install (NodeContainer (switchA, switchB));
     switchADevices.Add(link.Get(0));
     switchBDevices.Add(link.Get(1));
  }

  // Link from n2 <-> B

  {
     NetDeviceContainer link = csma.Install (NodeContainer (terminalsB.Get(0), switchB));
     terminalDevices.Add (link.Get (0));
     switchBDevices.Add(link.Get(1));
  }

  // Create the bridge netdevice, which will do the packet switching
  Ptr<Node> switchANode = switchA.Get(0);
  BridgeHelper bridgeA;
  bridgeA.Install (switchANode, switchADevices);

  Ptr<Node> switchBNode = switchB.Get(0);
  BridgeHelper bridgeB;
  bridgeB.Install (switchBNode, switchBDevices);

  // Setup the terminals as Internet hosts.
  // And provide them with IP addresses.

  InternetStackHelper internet;
  internet.Install (terminalsA);
  internet.Install (terminalsB);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = ipv4.Assign (terminalDevices);

  //
  // Create UDP Applications to send the two packets
  //

  NS_LOG_INFO ("Create Applications.");

  uint16_t port = 9;   // Discard port (RFC 863)

  UdpClientHelper udpClientHelperA (interfaces.GetAddress (1), port);
  udpClientHelperA.SetAttribute ("MaxPackets", UintegerValue (1));

  ApplicationContainer udpClientA = udpClientHelperA.Install (terminalsA.Get(0));	// n0 -> n1
  udpClientA.Start (Seconds (1.0));
  udpClientA.Stop  (Seconds (3.0));

  UdpClientHelper udpClientHelperB (interfaces.GetAddress (2), port);
  udpClientHelperB.SetAttribute ("MaxPackets", UintegerValue (1));

  ApplicationContainer udpClientB = udpClientHelperB.Install (terminalsA.Get(0));	// n0 -> n2
  udpClientB.Start (Seconds (6.0));
  udpClientB.Stop  (Seconds (8.0));

  NodeContainer serverNodes;
  serverNodes.Add(terminalsA.Get(1)); // n1
  serverNodes.Add(terminalsB.Get(0)); // n2

  UdpServerHelper udpServerHelper (port);
  ApplicationContainer udpServers = udpServerHelper.Install (serverNodes);
  udpServers.Start (Seconds (0.0));

  // Give us an ASCII trace of what happens.

  NS_LOG_INFO ("Configure Tracing.");

  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("moose-csma-bridge.tr"));

  // Uses CSMA helper - may want to change this to something more useful (e.g. a bridge tracing??)

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
