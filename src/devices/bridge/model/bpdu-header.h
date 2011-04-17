/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005 INRIA
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

#ifndef BRIDGE_BPDU_HEADER_H
#define BRIDGE_BPDU_HEADER_H

#include "ns3/header.h"
#include <string>

namespace ns3 {
/**
 * \ingroup bridge
 * \brief The packet header for an Bridge Protocol Data Unit (BPDU) packet header
 */

class BpduHeader : public Header {

public:

	uint16_t GetProtocol();
	uint8_t GetVersion();
	uint8_t GetType();

	void SetProtocol(uint16_t);
	void SetVersion(uint8_t);
	void SetType(uint8_t);

	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);

private:

	uint16_t m_protocolIdentifier;
	uint8_t m_versionIdentifier;
	uint8_t m_bpduType;


};

/**
 * \ingroup bridge
 * \brief The packet header for an Configuration BPDU packet header
 */


class ConfigurationBpduHeader : public Header {

public:

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

};

/**
 * \ingroup bridge
 * \brief The packet header for an Topology Change Notification BPDU packet header
 */


class TcnBpduHeader : public Header {

public:

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);


};

/**
 * \ingroup bridge
 * \brief The packet header for an Rapid Spanning Tree BPDU packet header
 */


class RstBpduHeader : public Header {

};

}

#endif

