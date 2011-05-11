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

#include "tree.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("TreeTopologyHelper");

namespace ns3 {

Topology TreeTopologyHelper::Create(long hosts, long branch, long depth){

	Topology t;

	long level = 1;
	long bridges = 1;

	t.bridges = 0;

	while(level < depth){

		for(long i = 0; i < bridges; ++i){
			for(long j = 0; j < branch; ++j){
				t.bridgeLinks.insert(std::make_pair<long,long>( t.bridges + i , t.bridges + bridges + i*bridges + j ));
			}
		}

		t.bridges += bridges;
		level ++;
		bridges = bridges * branch;
	}

	for(long k = 0; k < bridges; ++k){
		for(long l = 0; l < hosts; ++l){
			t.hostLinks[(k*hosts) + l] = t.bridges + k;
		}
	}

	t.bridges += bridges;
	t.hosts = bridges * hosts;

	return t;

}


}

