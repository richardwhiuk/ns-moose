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

	parseState();

	analyseCsma();

	analyseState();

	generateDot();

}

Analysis::~Analysis(){

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

void Analysis::parseState(){
	std::string line;
	long size;

	getline(state, line);
	assert(line == "ns-moose");	// MAGIC
	
	getline(state, line);
	assert(line == "3");		// TYPE
	
	getline(state, line);
	assert(line == "1");		// VERSION
	
	do {
		getline(state, line);		// LINK TYPE
	} while(line == "");
	
	while(!state.eof()){

		assert( (line == "1" && !moose) || (line == "2" && moose) );

		stateParse *sp;

		if(moose){
			mooseStateParse *msp = new mooseStateParse;

			getline(state, msp->mac);
			getline(state, msp->moose);

			getline(state, line);		// Size
			
			{
				std::stringstream sline(line);
				sline >> size;
			}

			for(; size > 0; size --){
				switchParse sp;
				getline(state, sp.prefix);
				getline(state, sp.port);
				getline(state, sp.time);
				msp->switches.push_back(sp);
			}

			getline(state, line);		// Size
			
			{
				std::stringstream sline(line);
				sline >> size;
			}


			for(; size > 0; size --){
				hostParse hp;
				getline(state, hp.mac);
				getline(state, hp.suffix);
				getline(state, hp.port);
				getline(state, hp.time);
				msp->hosts.push_back(hp);
			}

			sp = msp;

		} else {
			ethStateParse *esp = new ethStateParse;
		
			getline(state, esp->mac);	// MAC

			getline(state, line);		// Size

			{
				std::stringstream sline(line);
				sline >> size;
			}

			for(; size > 0; size --){
				entryParse ep;
				getline(state, ep.mac);
				getline(state, ep.port);
				getline(state, ep.time);
				esp->entries.push_back(ep);
			}
			
			sp = esp;
		}

		states.push_back(sp);

		do {
			getline(state, line);		// LINK TYPE
		} while(line == "" && !state.eof());
	}
}

void Analysis::analyseState(){

	output << "State Table " << std::endl;

	for(std::vector<stateParse*>::iterator it = states.begin(); it != states.end(); ++it){

		if(moose){
			mooseStateParse *msp = (mooseStateParse *) *it;
			output 	<< "\t" << msp->moose << std::endl
				<< "\t\t Hosts \t" << msp->hosts.size() << std::endl
				<< "\t\t Switches \t" << msp->switches.size() << std::endl
				<< "\t\t Total \t" << (msp->hosts.size() + msp->switches.size()) << std::endl
				<< std::endl;
		} else {
			ethStateParse *esp = (ethStateParse *)*it;
			output 	<< "\t" << esp->mac << std::endl
				<< "\t\t Entries \t" << esp->entries.size() << std::endl
				<< std::endl;
		}

	}

	output << std::endl;

}

void Analysis::analyseCsmaType(csmaParse& parse, csmaAnalysis& analysis){
	if(parse.type == "+"){
		++(analysis.added);
	} else if(parse.type == "-"){
		++(analysis.removed);
	} else if(parse.type == "r"){
		++(analysis.received);
	} else if(parse.type == "d"){
		++(analysis.dropped);
	} else {
		std::cerr << parse.type << std::endl;
		assert(false);
	}
}

void Analysis::analyseCsmaLine(csmaParse& it){

	std::map<std::string, std::map<std::string, std::string> >::iterator mit;

	if((mit = it.packet.find("ns3::ArpHeader")) != it.packet.end()){
		if(it.packet["ns3::EthernetHeader"]["destination"] == "ff:ff:ff:ff:ff:ff"){
			analyseCsmaType(it, arp.request);
		} else {
			analyseCsmaType(it, arp.response);
		}

		analyseCsmaType(it, arp.total);

	} else if(
			((mit = it.packet.find("ns3::Ipv4Header")) != it.packet.end()) 
			&& 
			((mit = it.packet.find("ns3::UdpHeader")) != it.packet.end())
		) {

		analyseCsmaType(it, udp);

	} else {
		mit = it.packet.begin(); ++mit;
		std::cerr << mit->first << std::endl;
		assert(false);
	}

	if(it.packet["ns3::EthernetHeader"]["destination"] == "ff:ff:ff:ff:ff:ff"){
		analyseCsmaType(it, broadcast);
	} else {
		analyseCsmaType(it, unicast);
	}

	analyseCsmaType(it, total);

}

void Analysis::analyseCsma(){

	output 	<< "ARP " << std::endl
		<< "\t Request \t" << arp.request << std::endl
		<< "\t Response \t" << arp.response << std::endl
		<< "\t Total \t" << arp.total << std::endl 
		<< std::endl
		<< "UDP \t" << udp << std::endl 
		<< std::endl
		<< "Unicast \t" << unicast << std::endl
		<< "Broadcast \t" << broadcast << std::endl
		<< std::endl
		<< "Total \t" << total << std::endl
		<< std::endl;

}

std::ostream& operator<<(std::ostream& file, Analysis::csmaAnalysis& analysis){
	file 	<< "+: " << analysis.added 
		<< " -: " << analysis.removed 
		<< " r: " << analysis.received
		<< " d: " << analysis.dropped
			;
	return file;
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
	
	analyseCsmaLine(parse);

}

}

