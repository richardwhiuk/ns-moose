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
#ifndef MOOSE_BRIDGE_NET_DEVICE_H
#define MOOSE_BRIDGE_NET_DEVICE_H

#include "bridge-net-device.h"
#include "ns3/moose-address.h"
#include "ns3/moose-prefix-address.h"
#include "ns3/moose-suffix-address.h"

namespace ns3 {

/**
 * \ingroup bridge
 * \brief a virtual net device that bridges multiple LAN segments
 * 
 * MOOSE (Multi-Level Origin Organised Scalable Ethernet) Implementation
 */
class MooseBridgeNetDevice : public BridgeNetDevice
{

public:
  static TypeId GetTypeId (void);
  MooseBridgeNetDevice ();
  virtual ~MooseBridgeNetDevice ();

  void Learn(MooseAddress const& addr, Ptr<BridgePortNetDevice> port);
  Ptr<BridgePortNetDevice> GetLearnedPort(MooseAddress const& addr);
  MooseAddress ToMoose(MooseAddress const& addr);
  MooseAddress FromMoose(MooseAddress const& addr);

  void AddRoutes(std::map<MoosePrefixAddress, Ptr<BridgePortNetDevice> > routes);

  void SetMoosePrefixAddress(MoosePrefixAddress const& prefix);
  MoosePrefixAddress GetMoosePrefixAddress();

protected:

  virtual void ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
  virtual void ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst)

  virtual Ptr<BridgePortNetDevice> CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node);

private:

// Need more types of state

  MooseAddress m_mooseAddress;

  // Remote Moose State

  struct PrefixState
  {
    Ptr<BridgePortNetDevice> associatedPort;
    Time expirationTime;
  };  

  // Local Moose State

  struct SuffixState
  {
    Mac48Address ethernet;
    MooseSuffixAddress suffix;
    Time expirationTime;
  };  

  struct PortState 
  {
    Ptr<BridgePortNetDevice> associatedPort;
    Time expirationTime;
  };
 
  Time m_expirationTime;

  std::map<MoosePrefixAddress, PrefixState> m_prefixState;
  std::map<Mac48Address, SuffixState> m_suffixState;
  std::map<MooseSuffixAddress, SuffixState*> m_ethernetState;
  std::map<MooseSuffixAddress, PortState> m_portState;

};

} // namespace ns3

#endif
