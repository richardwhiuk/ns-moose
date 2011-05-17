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

#include "analysis.h"
#include "ns3/object.h"
#include "ns3/log.h"

#include <assert.h>

#include <stdexcept>
#include <iostream>
#include <fstream>

NS_LOG_COMPONENT_DEFINE ("Analysis");

namespace ns3 {

Analysis::Analysis(std::istream& topology, std::istream& data, std::istream& csma, std::istream& state, std::ostream& output, std::ostream& dot, std::ostream& graph, bool moose) : topology(topology), data(data), csma(csma), state(state), output(output), dot(dot), graph(graph), moose(moose){

	parseCsma();

	analyseCsma();

	generateDot();

}

Analysis::~Analysis(){

}

void Analysis::printCsma(){
	long i = 0;
	for(std::vector<csmaParse>::iterator it = traces.begin(); it != traces.end(); ++it, ++i){
		std::cout << "[" << i << "]" << std::endl;
		std::cout << '\t' << it->type << std::endl;
		std::cout << '\t' << it->time << std::endl;
		std::cout << '\t' << it->node << std::endl;
		for(std::map<std::string,std::map<std::string,std::string> >::iterator vit = it->packet.begin(); vit != it->packet.end(); ++vit){
			std::cout << '\t' << vit->first << std::endl;
			for(std::map<std::string, std::string>::iterator mit = vit->second.begin(); mit != vit->second.end(); ++mit){
				std::cout << "\t\t" << mit->first << '\t' << mit->second << std::endl; 
			}
		}
	}
	
}

void Analysis::generateDot(){
	dot << "graph G { " << std::endl;
        dot << "node [shape=box, style=filled];" << std::endl;
	for(long i = 0; i < topology.bridges; ++i){
		dot << "S" << i << ";" << std::endl;
	}
        dot << "node [shape=circle, style=filled];" << std::endl;
	for(long i = 0; i < topology.hosts; ++i){
        	dot << "H" << i << ";" << std::endl;
        }
	dot << "edge [len=1];" << std::endl;
        for(Topology::HostLinks::iterator it = topology.hostLinks.begin(); it != topology.hostLinks.end(); ++it){
		dot << "H" << it->first << " -- S" << it->second << "; " << std::endl;
	}
        dot << "edge [len=2];" << std::endl;
        for(Topology::BridgeLinks::iterator it = topology.bridgeLinks.begin(); it != topology.bridgeLinks.end(); ++it){
		dot << "S" << it->first << " -- S" << it->second << "; " << std::endl;
	}
	dot << "}" << std::endl;

}

void Analysis::parseCsma(){
	std::string line;
	while(!csma.eof()){
		getline(csma, line);
		if(line.size() > 0){
			parseCsmaLine(line);
		}
	}
}

void Analysis::analyseCsma(){

	for(std::vector<csmaParse>::iterator it = traces.begin(); it != traces.end(); ++it){
		std::map<std::string, std::map<std::string, std::string> >::iterator mit;
		if((mit = it->packet.find("ns3::ArpHeader")) != it->packet.end()){
			if(it->type == "+"){
				++(arp.added);
			} else if(it->type == "-"){
				++(arp.removed);
			} else if(it->type == "r"){
				++(arp.received);
			}		

		} else if(
				((mit = it->packet.find("ns3::Ipv4Header")) != it->packet.end()) 
				&& 
				((mit = it->packet.find("ns3::UdpHeader")) != it->packet.end())
			) {
			if(it->type == "+"){
				++(udp.added);
			} else if(it->type == "-"){
				++(udp.removed);
			} else if(it->type == "r"){
				++(udp.received);
			}		
		} else {
			mit = it->packet.begin(); ++mit;
			std::cout << mit->first << std::endl;
			assert(false);
		}

		if(it->packet["ns3::EthernetHeader"]["destination"] == "ff:ff:ff:ff:ff:ff"){
			if(it->type == "+"){
				++(broadcast.added);
			} else if(it->type == "-"){
				++(broadcast.removed);
			} else if(it->type == "r"){
				++(broadcast.received);
			}		
		} else {
			if(it->type == "+"){
				++(unicast.added);
			} else if(it->type == "-"){
				++(unicast.removed);
			} else if(it->type == "r"){
				++(unicast.received);
			}		
		}

		if(it->type == "+"){
			++(total.added);
		} else if(it->type == "-"){
			++(total.removed);
		} else if(it->type == "r"){
			++(total.received);
		} else {
			std::cout << it->type << std::endl;
			assert(false);
		}
	}

	std::cout << "ARP +: " << arp.added << " -: " << arp.removed << " r: " << arp.received << std::endl;
	std::cout << "UDP +: " << udp.added << " -: " << udp.removed << " r: " << udp.received << std::endl;

	std::cout << "UCa +: " << unicast.added << " -: " << unicast.removed << " r: " << unicast.received << std::endl;
	std::cout << "BCa +: " << broadcast.added << " -: " << broadcast.removed << " r: " << broadcast.received << std::endl;

	std::cout << "Total +: " << total.added << " -: " << total.removed << " r: " << total.received << std::endl;
	

	//std::vector<std::pair<std::string, std::string> >::iterator hit;

}

void Analysis::parseCsmaLine(std::string& line){
	csmaParse parse;
	size_t spaa, spab, spac, spad;
	size_t dend, dsta, dstb, dmid;
	size_t sepa, sepb;

	// type sepa time sepb node sepc { header sepd line 

	spaa = line.find(' ');
	parse.type = line.substr(0, spaa);
	spab = line.find(' ', spaa + 1);
	parse.time = line.substr(spaa + 1, spab - spaa - 1);
	spac = line.find(' ', spab + 1);
	parse.node = line.substr(spab + 1, spac - spab - 1);

	while(spac != std::string::npos){
		std::string header;
		std::map<std::string, std::string> data;

		spad = line.find(' ', spac + 1);
		header = line.substr(spac + 1, spad - spac - 1);

		dsta = line.find('(', spad + 1);
		dstb = line.find(' ', dsta + 1);
		dsta = (dstb == (dsta + 1)) ? dstb : dsta;

		dend = line.find(')', dsta + 1);

		dmid = dsta;

		while(dmid < dend){

			std::string key;		
	
			sepa = line.find('=', dmid + 1);

			if(sepa <= dend){
				key = line.substr(dmid + 1, sepa - dmid - 1);
				
				sepb = line.find(',', sepa + 1);
				if(sepb <= dend){
					data[key] = line.substr(sepa + 1, sepb - sepa - 1);
					dmid = sepb + 1;
				} else {
					data[key] = line.substr(sepa + 1, dend - sepa - 1);
					dmid = dend;
				}
			} else {
				sepa = line.find(": ", dmid + 1);

				if(sepa <= dend){

					key = line.substr(dmid + 1, sepa - dmid - 1);

					sepb = line.find(' ', sepa + 2);

					if(sepb <= dend){
						data[key] = line.substr(sepa + 2, sepb - sepa - 2);
						dmid = sepb;
					} else {
						data[key] = line.substr(sepa + 2, dend - sepa - 2);
						dmid = dend;
					}

				} else {

					data["route"] = line.substr(dmid + 1, dend - dmid - 1);

					dmid = dend;

				}

			}



		}

		spac = line.find(' ', dend + 1);

		if(parse.packet.find(header) != parse.packet.end()){
			std::cout << line << std::endl;
		}

		parse.packet[header] = data;

	}
	
	traces.push_back(parse);

}

}

