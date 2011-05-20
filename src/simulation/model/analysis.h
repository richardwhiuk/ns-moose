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

private:
	struct csmaParse;
	struct csmaAnalysis;
	struct csmaArpAnalysis;

public:

	Analysis(std::istream& topology, std::istream& data, std::istream& csma, std::istream& state, std::ostream& output, std::ostream& dot, std::ostream& graph, bool moose);
	virtual ~Analysis();

protected:

	void generateDot();

	void analyseCsma();
	void analyseState();
	
	void printCsma();

	void parseCsma();
	void parseState();

private:

	void analyseCsmaType(csmaParse& parse, csmaAnalysis& analysis);
	void parseCsmaLine(std::string& line);
	void analyseCsmaLine(csmaParse&);

	struct csmaParse {
		std::string type;
		std::string time;
		std::string node;
		std::map<std::string,std::map<std::string,std::string> > packet;
	};

	struct csmaAnalysis {

		csmaAnalysis() : added(0), removed(0), received(0), dropped(0){
	
		}
	
		long added;
		long removed;
		long received;
		long dropped;

		friend std::ostream& operator<<(std::ostream&, csmaAnalysis&);

	};

	struct csmaArpAnalysis {
		csmaAnalysis request;
		csmaAnalysis response;
		csmaAnalysis total;
	};

	struct stateParse {
		std::string mac;
	};

	struct entryParse {
		std::string mac;
		std::string port;
		std::string time;
	};

	struct switchParse {
		std::string prefix;
		std::string port;
		std::string time;
	};

	struct hostParse {
		std::string mac;
		std::string suffix;
		std::string port;
		std::string time;
	};

	struct ethStateParse : public stateParse {
		std::vector<entryParse> entries;
	};

	struct mooseStateParse : public stateParse {
		std::string moose;
		std::vector<hostParse> hosts;
		std::vector<switchParse> switches;
	};

	csmaArpAnalysis arp;
	csmaAnalysis udp;
	csmaAnalysis total;
	
	csmaAnalysis broadcast;
	csmaAnalysis unicast;

	std::vector<stateParse*> states;

	Topology topology;

	std::istream& data;
	std::istream& csma;
	std::istream& state;

	std::ostream& output;
	std::ostream& dot;
	std::ostream& graph;

	bool moose;

	friend std::ostream& operator<<(std::ostream&, csmaAnalysis&);

};

std::ostream& operator<<(std::ostream&, Analysis::csmaAnalysis&);

}

#endif

