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

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <fstream>
#include <map>
#include <set>

namespace ns3 {

/** 
 * \ingroup topology
 * 
 * \brief Represents a topology
 */

struct Topology {
	
public:

	Topology();
	Topology(std::istream& file);
	~Topology();

	long bridges;
	long hosts;
	typedef std::map<long,long> HostLinks;
	typedef std::pair<long,long> BridgeLink;

	struct BridgeLinkCompare {
		// we don't care about ordering inside a pair, but we do care about pair order

		bool operator()( BridgeLink const &lhs, BridgeLink const &rhs);

	};
	
	typedef std::set<BridgeLink, BridgeLinkCompare> BridgeLinks;

	HostLinks hostLinks;
	BridgeLinks bridgeLinks;

};

std::ostream& operator<<(std::ostream&, ns3::Topology);

}

#endif

