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
#ifndef BRIDGE_NET_DEVICE_H
#define BRIDGE_NET_DEVICE_H

#include "bridge-net-device.h"
#include "ethernet-bridge-port-net-device.h"
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
 * \ingroup devices
 * \defgroup bridge Bridge
 * 
 * \brief a virtual net device that bridges multiple LAN segments
 *
 * The BridgeNetDevice object is a "virtual" netdevice that aggregates
 * multiple "real" netdevices and implements the data plane forwarding
 * part of IEEE 802.1D.  By adding a BridgeNetDevice to a Node, it
 * will act as a "bridge", or "switch", to multiple LAN segments.
 * 
 * By default the bridge netdevice implements a "learning bridge"
 * algorithm (see 802.1D), where incoming unicast frames from one port
 * may occasionally be forwarded throughout all other ports, but
 * usually they are forwarded only to a single correct output port.
 *
 * \attention The Spanning Tree Protocol part of 802.1D is not
 * implemented.  Therefore, you have to be careful not to create
 * bridging loops, or else the network will collapse.
 *
 * \attention Bridging is designed to work only with NetDevices
 * modelling IEEE 802-style technologies, such as CsmaNetDevice and
 * WifiNetDevice.
 *
 * \attention If including a WifiNetDevice in a bridge, the wifi
 * device must be in Access Point mode.  Adhoc mode is not supported
 * with bridging.
 */

/**
 * \ingroup bridge
 * \brief a virtual net device that bridges multiple LAN segments
 */
class BridgeNetDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);
  BridgeNetDevice ();
  virtual ~BridgeNetDevice ();

  /** 
   * \brief Add a 'port' to a bridge device
   *
   * This method adds a new bridge port to a BridgeNetDevice, so that
   * the new bridge port NetDevice becomes part of the bridge and L2
   * frames start being forwarded to/from this NetDevice.
   *
   * \param bridgePort NetDevice
   * \attention The netdevice that is being added as bridge port must
   * _not_ have an IP address.  In order to add IP connectivity to a
   * bridging node you must enable IP on the BridgeNetDevice itself,
   * never on its port netdevices.
   */
  Ptr<BridgePortNetDevice> AddBridgePort (Ptr<NetDevice> bridgePort);

  uint32_t GetNBridgePorts (void) const;

  Ptr<NetDevice> GetBridgePort (uint32_t n) const;

  // inherited from NetDevice base class.
  virtual void SetIfIndex(const uint32_t index);
  virtual uint32_t GetIfIndex(void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;

  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual void SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom () const;
  virtual Address GetMulticast (Ipv6Address addr) const;

  virtual void Forward (Ptr<BridgePortNetDevice> port, Ptr<const Packet> packet, uint16_t protocol, Address const &src, Address const &dst, PacketType packetType);

protected:
  virtual void DoDispose (void);

  virtual void ForwardUnicast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst) = 0;
  virtual void ForwardBroadcast (Ptr<BridgePortNetDevice> incomingPort, Ptr<const Packet> packet, uint16_t protocol, Mac48Address src, Mac48Address dst);

  virtual Ptr<BridgePortNetDevice> CreateBridgePort(Ptr<BridgeNetDevice> bridge, Ptr<NetDevice> device, Ptr<Node> node) = 0;

  virtual std::ostream& Print(std::ostream&) = 0;

  Mac48Address m_macAddress;

private:
  NetDevice::ReceiveCallback m_rxCallback;
  NetDevice::PromiscReceiveCallback m_promiscRxCallback;

  Ptr<Node> m_node;
  Ptr<BridgeChannel> m_channel;

  std::vector<Ptr<BridgePortNetDevice> > m_ports;

  uint16_t mPortNumber;
  uint32_t m_ifIndex;
  uint16_t m_mtu;
  bool m_enableLearning;

  friend std::ostream& operator<<(std::ostream&, BridgeNetDevice&);

};

std::ostream& operator<<(std::ostream&, BridgeNetDevice&);

} // namespace ns3

#endif /* BRIDGE_NET_DEVICE_H */
