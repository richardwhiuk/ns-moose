/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA
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
#ifndef MOOSE_ADDRESS_H
#define MOOSE_ADDRESS_H

#include "moose-prefix-address.h"
#include "moose-suffix-address.h"
#include "mac48-address.h"

namespace ns3 {

class Address;

/**
 * \ingroup address
 * 
 * \brief a MOOSE (Multi-Level Object Orientated Ethernet) address
 *
 * This class can contain 48 bit MOOSE addresses.
 */
class MooseAddress
{
public:
  MooseAddress ();
  MooseAddress (const char *str);
  MooseAddress (const Mac48Address &mac);

  enum MooseType {
     MOOSE,
     HOST,
     GROUP
  };

  /**
   * \returns the MooseType of this address. 
   */

  MooseType GetMooseType() const;

  /**
   * \returns the MOOSE prefix
   */

  const MoosePrefixAddress GetMoosePrefix() const;

  /**
   * \returns the MOOSE prefix
   */

  const MooseSuffixAddress GetMooseSuffix() const;

  /**
   * \returns the Mac48Address
   */

  const Mac48Address GetMacAddress() const;

  /**
   * Allocate a new Moose Bridge Address.
   */
  static MooseAddress Allocate ();

  /**
   * Allocate a new Moose Host Address.
   */
  static MooseAddress Allocate (MoosePrefixAddress prefix);

  /**
   * Form a Moose Address from Parts.
   */

  static MooseAddress Combine (MoosePrefixAddress prefix, MooseSuffixAddress suffix);

private:

  uint8_t m_address[6];

};

}

#endif

