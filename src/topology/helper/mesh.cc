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

#include "mesh.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("MeshTopologyHelper");

namespace ns3 {

Topology MeshTopologyHelper::Create(long hosts, long size){

	Topology t;

	t.bridges = size;

	t.hosts = t.bridges * hosts;

	long i; long j; long k;

	for(i = 0; i < t.bridges; ++i){
		for(j = 0; j < hosts; ++j){
			t.hostLinks[(i*hosts) + j] = i;
		}
	}

	for(i = 0; i < size; ++i){
		for(j = i + 1; j < size; ++j){
			t.bridgeLinks.insert(std::make_pair<long,long>(i,j));
		}	
	}

	return t;

}


}

