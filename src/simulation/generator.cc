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

#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MooseGenerator");

int main (int argc, char *argv[])
{

try {

	std::string type;
	unsigned long size = 0;
	unsigned long hosts = 0;
	std::string file;

	CommandLine cmd;			// Allow CommandLine args
	cmd.AddValue("file", "Output file", file);
	cmd.AddValue("type", "Network Topology (cube|mesh|torus)", type);
	cmd.AddValue("size", "Network size", size);
	cmd.AddValue("hosts", "Hosts per switch", hosts);
	cmd.Parse (argc, argv);

	Topology t;

	if(hosts == 0){
		throw new std::runtime_error("Invalid number of hosts");
	}

	if(size < 2){
		throw new std::runtime_error("Invalid network size");
	}

	if(type == "cube"){
		t = CubeTopologyHelper::Create(hosts, size);
	} else if(type == "mesh"){
		t = MeshTopologyHelper::Create(hosts, size);
	} else if(type == "torus"){
		t = TorusTopologyHelper::Create(hosts, size);
	} else {
		throw new std::runtime_error("Unknown network topology type");
	} 

	std::ofstream stream(file.c_str(), std::ios_base::out | std::ios_base::trunc);

	stream << t;

} catch(std::runtime_error* e) {
	std::cerr << e->what() << std::endl;
	return 1;
}

return 0;

}

