/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c)
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

#include "moose-bridge-helper.h"
#include "ns3/log.h"
#include "ns3/moose-bridge-net-device.h"
#include "ns3/node.h"
#include "ns3/names.h"
#include "ns3/moose-address.h"

NS_LOG_COMPONENT_DEFINE ("MooseBridgeHelper");

namespace ns3 {

MooseBridgeHelper::MooseBridgeHelper ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.SetTypeId ("ns3::MooseBridgeNetDevice");
}

void 
MooseBridgeHelper::SetDeviceAttribute (std::string n1, const AttributeValue &v1)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_deviceFactory.Set (n1, v1);
}

NetDeviceContainer
MooseBridgeHelper::Install (Ptr<Node> node, NetDeviceContainer c, MoosePrefixAddress addr, std::multimap<Ptr<NetDevice>, MoosePrefixAddress> routes)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("**** Install MOOSE bridge device on node " << node->GetId ());

  SetDeviceAttribute("MooseAddress", MooseAddressValue(MooseAddress::Allocate(addr)));

  NetDeviceContainer devs;
  Ptr<MooseBridgeNetDevice> dev = m_deviceFactory.Create<MooseBridgeNetDevice> ();
  devs.Add (dev);
  node->AddDevice (dev);

  std::map<MoosePrefixAddress, Ptr<BridgePortNetDevice> > portMap;

  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NS_LOG_LOGIC ("**** Add MooseBridgePort "<< *i);
      Ptr<BridgePortNetDevice> port = dev->AddBridgePort (*i);
      for(std::multimap<Ptr<NetDevice>, MoosePrefixAddress>::iterator it = routes.lower_bound(*i); it != routes.upper_bound(*i); ++it){
           portMap[it->second] = port;
      }
    }

  dev->AddRoutes(portMap);

  return devs;
}


NetDeviceContainer
MooseBridgeHelper::Install (Ptr<Node> node, NetDeviceContainer c)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("**** Install MOOSE bridge device on node " << node->GetId ());

  SetDeviceAttribute("MooseAddress", MooseAddressValue(MooseAddress::Allocate()));

  NetDeviceContainer devs;
  Ptr<BridgeNetDevice> dev = m_deviceFactory.Create<MooseBridgeNetDevice> ();
  devs.Add (dev);
  node->AddDevice (dev);

  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NS_LOG_LOGIC ("**** Add MooseBridgePort "<< *i);
      dev->AddBridgePort (*i);
    }
  return devs;
}

NetDeviceContainer
MooseBridgeHelper::Install (std::string nodeName, NetDeviceContainer c)
{
  NS_LOG_FUNCTION_NOARGS ();
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return Install (node, c);
}

} // namespace ns3

