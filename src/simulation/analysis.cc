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

#include "ns3/core-module.h"
#include "ns3/topology-module.h"
#include "ns3/simulation-module.h"

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MooseAnalysis");

int main (int argc, char *argv[])
{

try {

	std::string topology;
	std::string data;
	std::string csma;
	std::string state;
	std::string link = "ethernet";
	std::string output;
	std::string dot;
	std::string graph;

	{
		CommandLine cmd;			// Allow CommandLine args
		cmd.AddValue("topology", "Network topology", topology);
		cmd.AddValue("data", "Network data", data);
		cmd.AddValue("csma", "Network trace", csma);
		cmd.AddValue("state", "Bridge state", state);
		cmd.AddValue("link", "Link layer used (moose|ethernet) [ethernet]", link);
		cmd.AddValue("output", "Output file of analysis", output);
		cmd.AddValue("dot", ".dot file shwoing topology", dot);
		cmd.AddValue("graph", "gnuplot data file", graph);
		cmd.Parse (argc, argv);
	}

	// Setup streams

	std::ifstream topologyStream(topology.c_str(), std::ios_base::in);
	std::ifstream dataStream(data.c_str(), std::ios_base::in);
	std::ifstream csmaStream(csma.c_str(), std::ios_base::in);
	std::ifstream stateStream(state.c_str(), std::ios_base::in);

	std::ofstream outputStream(output.c_str(), std::ios_base::out | std::ios_base::trunc);
	std::ofstream dotStream(dot.c_str(), std::ios_base::out | std::ios_base::trunc);
	std::ofstream graphStream(graph.c_str(), std::ios_base::out | std::ios_base::trunc);

	// Check streams

        if(topologyStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open network topology file [" << topology << "]";
                throw new std::runtime_error(ss.str());
        }

        if(dataStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open network data file [" << data << "]";
                throw new std::runtime_error(ss.str());
        }

        if(csmaStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open network trace file [" << csma << "]";
                throw new std::runtime_error(ss.str());
        }

        if(stateStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open network state file [" << data << "]";
                throw new std::runtime_error(ss.str());
        }

        if(outputStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open output file [" << output << "]";
                throw new std::runtime_error(ss.str());
        }

        if(dotStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open dot file [" << dot << "]";
                throw new std::runtime_error(ss.str());
        }

        if(graphStream.fail()){
                std::ostringstream ss;
                ss << "Failed to open graph data file [" << graph << "]";
                throw new std::runtime_error(ss.str());
        }

	bool moose;

	if(link == "moose"){
		moose = true;
	} else if(link == "ethernet") {
		moose = false;
	} else {
                std::ostringstream ss;
                ss << "Unknown link layer: [" << link << "]";
                throw new std::runtime_error(ss.str());
	}

	// Analysis

	Analysis a(topologyStream, dataStream, csmaStream, stateStream, outputStream, dotStream, graphStream, moose);

} catch(std::runtime_error* e) {
	std::cerr << e->what() << std::endl;
	return 1;
}

return 0;

}

