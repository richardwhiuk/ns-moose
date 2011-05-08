
#ifndef RSTP_L3_PROTOCOL_H
#define RSTP_L3_PROTOCOL_H

#include "ethernet-bridge-net-device.h"

namespace ns3 {

/**
 * \ingroup bridge
 * \brief Implement the Rapid Spanning Tree Protocol (RSTP)
 *
 * This is the implementation of RSTP
**/

class RstpL3Protocol : public Object {

public:
	static TypeId GetTypeId (void);
	static const uint16_t PROT_NUMBER;
	
	RstpL3Protocol ();
	virtual ~RstpL3Protocol ();
	
	void SetNode(Ptr<Node> node);
	void SetBridge(Ptr<EthernetBridgeNetDevice> bridge);

	/**
	 * \brief Recieve a packet
	 */

	void Receive(Ptr<NetDevice> device, Ptr<const Packet> p, uint16_t protocol, const Address &from, const Address &to, NetDevice::PacketType packetType);

protected:

	virtual void DoDispose();
	virtual void NotifyNewAggregate();

private:

	RstpL3Protocol(const RstpL3Protocol &o);
	RstpL3Protocol &operator=(const RstpL3Protocol &o);

	Ptr<EthernetBridgeNetDevice> m_bridge;
	Ptr<Node> m_node;

};


}

#endif

