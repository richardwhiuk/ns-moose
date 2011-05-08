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
 * Author: Gustavo Carneiro  <gjc@inescporto.pt>
 * Author: Richard Whitehouse <ns3@richardwhiuk.com>
 */
#ifndef ETHERNET_BRIDGE_NET_DEVICE_H
#define ETHERNET_BRIDGE_NET_DEVICE_H

#include "bridge-net-device.h"
#include "ethernet-bridge-port-net-device.h"
#include "ethernet-bridge-state.h"
#include "ns3/net-device.h"
#include "ns3/mac48-address.h"
#include "ns3/nstime.h"
#include "ns3/bridge-channel.h"
#include "ns3/object-factory.h"
#include <stdint.h>
#include <string>
#include <map>

namespace ns3 {

class Node;

/**
 * \ingroup bridge
 * \brief a virtual net device that bridges multiple LAN segments
 */
class EthernetBridgeNetDevice : public BridgeNetDevice
{
public:
  static TypeId GetTypeId (void);
  EthernetBridgeNetDevice ();
  virtual ~EthernetBridgeNetDevice ();

  virtual void Forward (Ptr<BridgePortNetDevice> port, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, PacketType packetType);
  virtual void Learn (Address const &src, Ptr<BridgePortNetDevice> port);

protected:
  virtual void ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
  virtual void ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);
  virtual Ptr<BridgePortNetDevice> GetLearnedState (Mac48Address source);

  virtual Ptr<BridgePortNetDevice> CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node);

  virtual std::ostream& Print(std::ostream&);

private:
  Ptr<EthernetBridgeState> m_state;

  uint16_t mPortNumber;
  bool m_enableLearning;

};

} // namespace ns3

#endif /* ETHERNET_BRIDGE_NET_DEVICE_H */

