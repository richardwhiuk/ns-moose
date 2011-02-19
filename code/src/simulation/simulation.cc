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
//  ------------      ------------
//  | Switch 0 |------| Switch 2 |
//  ------------      ------------
//       |                  |
//  ------------      ------------
//  | Switch 1 |------| Switch 3 |
//  ------------      ------------
//       |
//       n2
//
// - 100 Mb/s
// - CBR/UDP flows from n0 to n1 and then from n0 to n2
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
  n.t.hosts = 3;

  // Link hosts to bridges

  n.t.hostLinks[0] = 0;
  n.t.hostLinks[1] = 0;
  n.t.hostLinks[2] = 1; 

  // Link the two bridges together
 
  n.t.bridgeLinks.insert(std::make_pair<long,long>(0,1));
  n.t.bridgeLinks.insert(std::make_pair<long,long>(0,2));
  n.t.bridgeLinks.insert(std::make_pair<long,long>(1,3));
  n.t.bridgeLinks.insert(std::make_pair<long,long>(2,3));

  // Create Network

  moose.Create(n);

  // TODO: Apps

  // Tracing

  NS_LOG_INFO ("Configure Tracing.");

  AsciiTraceHelper ascii;
  moose.csma.EnableAsciiAll (ascii.CreateFileStream ("moose-bridge.tr"));

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

}

