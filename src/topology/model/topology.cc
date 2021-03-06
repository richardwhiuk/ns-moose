/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008,2009 IITP RAS
 *
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

#include "topology.h"
#include "ns3/log.h"

#include <stdexcept>

NS_LOG_COMPONENT_DEFINE ("Topology");

namespace ns3 {

std::ostream& operator<<(std::ostream& file, ns3::Topology t){
	file << "ns-moose" << std::endl;
	file << 1 << std::endl;
	file << 1 << std::endl;
	file << t.hosts << std::endl;
	file << t.bridges << std::endl;
	for(Topology::BridgeLinks::iterator it = t.bridgeLinks.begin(); it != t.bridgeLinks.end(); it ++){
		file << it->first << '\t' << it->second << std::endl;
	}
	for(Topology::HostLinks::iterator it = t.hostLinks.begin(); it != t.hostLinks.end(); it ++){
		file << (it->first + t.bridges) << '\t' << it->second << std::endl;
	}
	return file;
}

Topology::Topology(){

}

Topology::Topology(std::istream& file){

	std::string line;

	std::getline(file, line);
	if(line != "ns-moose"){
		throw new std::runtime_error("Invalid Network Topology File");
	}

	unsigned long type;
	file >> type;
	if(type != 1){
		throw new std::runtime_error("Invalid Network Topology File Type");
	}

	unsigned long version;
	file >> version;
	if(version != 1){
		throw new std::runtime_error("Invalid Network Topology File Version");
	}

	file >> hosts;
	if(hosts < 1){
		throw new std::runtime_error("Network Topology contains no hosts.");
	}

	file >> bridges;
	if(bridges < 1){
		throw new std::runtime_error("Network Topology contains no bridges.");
	}

	while(file.good() && !file.eof()){
		unsigned long source;
		unsigned long destination;
		file >> source;
		file >> destination;
		if(file.good()){
			if(source >= bridges){				// Source is a host
				source -= bridges;
				if(source >= hosts){
					throw new std::runtime_error("Invalid Link in Topology");
				}
				if(destination >= bridges){		// Dest is a host
					throw new std::runtime_error("Host-Host Link in Topology");
				}
				hostLinks[source] = destination;
			} else {					// Source is a bridge
				if(destination >= bridges){		// Source is a host
					destination -= bridges;
					if(destination >= hosts){
						throw new std::runtime_error("Invalid Host Link");
					}
					hostLinks[destination] = source;
				} else {
					bridgeLinks.insert(Topology::BridgeLink(source, destination));
				}
			}
		}
	}

}

Topology::~Topology(){

}

bool Topology::BridgeLinkCompare::operator()( BridgeLink const &lhs, BridgeLink const &rhs) {
	if(lhs.first < lhs.second){
		if(rhs.first < rhs.second){
			if(lhs.first < rhs.first){
				return true;
			} else if(rhs.first < lhs.first){
				return false;
			} else {
				return lhs.second < rhs.second;
			}
		} else {
			if(lhs.first < rhs.second){
				return true;
			} else if(rhs.second < lhs.first){
				return false;
			} else {
				return lhs.second < rhs.first;
			}
		}
	} else {
		if(rhs.first < rhs.second){
			if(lhs.second < rhs.first){
				return true;
			} else if(rhs.first < lhs.second){
				return false;
			} else {
				return lhs.first < rhs.second;
			}
		} else {
			if(lhs.second < rhs.second){
				return true;
			} else if(rhs.second < lhs.second){
				return false;
			} else {
				return lhs.first < rhs.first;
			}
		}
	}
}

}

