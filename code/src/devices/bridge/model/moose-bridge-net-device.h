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

protected:

// From NetDevice

// Unconvinced we need these...

//  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
//  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);

// From BridgeNetDevice

/*

  virtual void Forward (Ptr<BridgePortNetDevice> port, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, PacketType packetType);
  virtual void ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
  virtual void ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
  virtual void Learn (Address const &src, Ptr<BridgePortNetDevice> port);
  virtual Ptr<BridgePortNetDevice> GetLearnedState (Mac48Address source); */

  virtual Ptr<BridgePortNetDevice> CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node);

private:

// Need more types of state

/**
  Local Port Mappings
	-- Implemented in parent.

  struct LearnedState
  {
    Ptr<BridgePortNetDevice> associatedPort;
    Time expirationTime;
  };
  std::map<Mac48Address, LearnedState> m_learnState;
**/

  MooseAddress m_mooseAddress;

  // Remote Moose State

  struct PrefixState
  {
    Ptr<BridgePortNetDevice> associatedPort;
    Time expirationTime;
  };
  std::map<MoosePrefixAddress, PrefixState> m_remoteState;

  // Local Moose State

  struct SuffixState
  {
    Mac48Address ethernetAddr;
    Time expirationTime;
  };
  std::map<MooseSuffixAddress, PrefixState> m_localState;

};

} // namespace ns3

#endif
