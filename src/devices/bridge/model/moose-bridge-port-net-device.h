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
#ifndef MOOSE_BRIDGE_PORT_NET_DEVICE_H
#define MOOSE_BRIDGE_PORT_NET_DEVICE_H

#include "bridge-port-net-device.h"
#include "ns3/net-device.h"
#include "ns3/mac48-address.h"
#include "ns3/moose-address.h"
#include "ns3/moose-prefix-address.h"
#include "ns3/moose-suffix-address.h"
#include "ns3/nstime.h"
#include <stdint.h>
#include <string>
#include <map>

namespace ns3 {

class Node;
class BridgeNetDevice;

/**
 * \ingroup bridge
 * \brief a port on a virtual net device that bridges multiple LAN segments
 */

class MooseBridgePortNetDevice : public BridgePortNetDevice  {

public:
  MooseBridgePortNetDevice(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node);
  virtual ~MooseBridgePortNetDevice();

  virtual void Send (Ptr<Packet> packet, const Address& src, const Address& dest, uint16_t protocolNumber);
  virtual void Receive (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, Address const &source, Address const &destination, NetDevice::PacketType packetType);

  static TypeId GetTypeId (void);

};

}

#endif

