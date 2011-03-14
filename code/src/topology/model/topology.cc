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

NS_LOG_COMPONENT_DEFINE ("MooseHelper");

namespace ns3 {

bool Topology::BridgeLinkCompare::operator()( BridgeLink const &lhs, BridgeLink const &rhs) {
	if(lhs.first < lhs.first){
		if(rhs.first < rhs.second){
			return (lhs.first < rhs.first) ? true : (lhs.second < rhs.second);
		} else {
			return (lhs.first < rhs.second) ? true : (lhs.second < rhs.first);
		}
	} else {
		if(rhs.first < rhs.second){
			return (lhs.second < rhs.first) ? true : (lhs.first < rhs.second);
		} else {
			return (lhs.second < rhs.second) ? true : (lhs.first < rhs.first);
		}
	}
}


}
