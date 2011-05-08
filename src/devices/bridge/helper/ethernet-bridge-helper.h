/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Author: Gustavo Carneiro <gjc@inescporto.pt>
 * Author: Richard Whitehouse <ns3@richardwhiuk.com>
 */
#ifndef ETHERNET_BRIDGE_HELPER_H
#define ETHERNET_BRIDGE_HELPER_H

#include "ns3/net-device-container.h"
#include "ns3/object-factory.h"
#include <string>
#include <map>
#include <assert.h>

namespace ns3 {

class Node;
class AttributeValue;

/**
 * \brief Add capability to bridge multiple LAN segments (IEEE 802.1D bridging)
 */
class EthernetBridgeHelper
{
public:
  /*
   * Construct a EthernetBridgeHelper
   */
  EthernetBridgeHelper ();
  /**
   * Set an attribute on each ns3::EthernetBridgeNetDevice created by
   * EthernetBridgeHelper::Install
   *
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   */
  void SetDeviceAttribute (std::string n1, const AttributeValue &v1);

  /**
   * This method creates an ns3::EthernetBridgeNetDevice with the attributes
   * configured by EthernetBridgeHelper::SetDeviceAttribute, adds the device
   * to the node, and attaches the given NetDevices as ports of the
   * bridge.
   *
   * This version uses static spanning tree to disable loops.
   *
   * \param node The node to install the device in
   * \param c Container of NetDevices to add as bridge ports
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (Ptr<Node> node, NetDeviceContainer c, std::map<Ptr<NetDevice>, bool> portsEnabled);

  /**
   * This method creates an ns3::EthernetBridgeNetDevice with the attributes
   * configured by EthernetBridgeHelper::SetDeviceAttribute, adds the device
   * to the node, and attaches the given NetDevices as ports of the
   * bridge.
   *
   * \param node The node to install the device in
   * \param c Container of NetDevices to add as bridge ports
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (Ptr<Node> node, NetDeviceContainer c);
  /**
   * This method creates an ns3::EthernetBridgeNetDevice with the attributes
   * configured by EthernetBridgeHelper::SetDeviceAttribute, adds the device
   * to the node, and attaches the given NetDevices as ports of the
   * bridge.
   *
   * \param nodeName The name of the node to install the device in
   * \param c Container of NetDevices to add as bridge ports
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (std::string nodeName, NetDeviceContainer c);
private:
  ObjectFactory m_deviceFactory;
};

} // namespace ns3


#endif /* ETHERNET_BRIDGE_HELPER_H */
