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

#ifndef SIMULATION_ANALYSIS
#define SIMULATION_ANALYSIS

#include "ns3/topology.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

namespace ns3 {

class Analysis {

public:

	Analysis(std::istream& topology, std::istream& data, std::istream& csma, std::istream& state, std::ostream& output, std::ostream& dot, std::ostream& graph, bool moose);
	virtual ~Analysis();

protected:

	void generateDot();
	void analyseCsma();
	void printCsma();
	void parseCsma();
	void parseCsmaLine(std::string& line);

private:

	struct csmaParse {
		std::string type;
		std::string time;
		std::string node;
		std::map<std::string,std::map<std::string,std::string> > packet;
	};

	struct csmaAnalysis {

		csmaAnalysis() : added(0), removed(0), received(0){
	
		}
	
		long added;
		long removed;
		long received;
	};

	csmaAnalysis arp;
	csmaAnalysis udp;
	csmaAnalysis total;
	
	csmaAnalysis broadcast;
	csmaAnalysis unicast;

	std::vector<csmaParse> traces;

	Topology topology;

	std::istream& data;
	std::istream& csma;
	std::istream& state;

	std::ostream& output;
	std::ostream& dot;
	std::ostream& graph;

	bool moose;

};

}

#endif

