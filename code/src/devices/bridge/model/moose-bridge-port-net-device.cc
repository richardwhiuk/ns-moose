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

#include "bridge-net-device.h"
#include "moose-bridge-net-device.h"
#include "bridge-port-net-device.h"
#include "moose-bridge-port-net-device.h"
#include "ns3/node.h"
#include "ns3/channel.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("MooseBridgePortNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MooseBridgePortNetDevice);

TypeId
MooseBridgePortNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MooseBridgePortNetDevice")
    .SetParent<BridgePortNetDevice> ()
    ;
  return tid;
}

MooseBridgePortNetDevice::MooseBridgePortNetDevice(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node) : BridgePortNetDevice(bridge,device,node){
  NS_LOG_FUNCTION_NOARGS ();
}

MooseBridgePortNetDevice::~MooseBridgePortNetDevice(){
  NS_LOG_FUNCTION_NOARGS ();
}

}


