
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "rstp-l3-protocol.h"

NS_LOG_COMPONENT_DEFINE("RstpL3Protocol");

namespace ns3 {

const uint16_t RstpL3Protocol::PROT_NUMBER = 0x0; // TODO

NS_OBJECT_ENSURE_REGISTERED (RstpL3Protocol);

TypeId RstpL3Protocol::GetTypeId(void){
	static TypeId tid = TypeId("ns3::RstpL3Protocol")
		.SetParent<Object> ()
		.AddConstructor<RstpL3Protocol> ();

	return tid;
}

RstpL3Protocol::RstpL3Protocol(){
	NS_LOG_FUNCTION(this);
}

RstpL3Protocol::~RstpL3Protocol(){
	NS_LOG_FUNCTION(this);
}

void RstpL3Protocol::SetBridge(Ptr<EthernetBridgeNetDevice> bridge){
	NS_LOG_FUNCTION(this);
	m_bridge = bridge;
}

void RstpL3Protocol::SetNode(Ptr<Node> node){
	NS_LOG_FUNCTION(this);
	m_node = node;
}

void RstpL3Protocol::DoDispose(){
	NS_LOG_FUNCTION(this);
	m_node = 0;
	Object::DoDispose();
}

void RstpL3Protocol::NotifyNewAggregate(){
	if(m_node == 0){
		Ptr<Node> node = this->GetObject<Node>();

		if(node != 0){
			this->SetNode(node);
		}
	}
	if(m_bridge == 0){
		Ptr<EthernetBridgeNetDevice> bridge = this->GetObject<EthernetBridgeNetDevice>();
		
		if(bridge != 0){
			this->SetBridge(bridge);
		}
	}
	Object::NotifyNewAggregate();
}

}

