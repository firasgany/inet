//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/protocol/ethernet/AddressChecking.h"
#include "inet/protocol/ethernet/EthernetHeaders_m.h"

namespace inet {

Define_Module(AddressChecking);

void AddressChecking::initialize(int stage)
{
    PacketFilterBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        interfaceEntry = findContainingNicModule(this);     //TODO or getContainingNicModule() ? or use a macaddresstable?
    }
}

void AddressChecking::handleParameterChange(const char *parName)
{
}

bool AddressChecking::matchesPacket(Packet *packet)
{
    const auto& header = packet->popAtFront<Ieee8023MacAddresses>();
    auto macAddressInd = packet->addTagIfAbsent<MacAddressInd>();
    auto destAddr = header->getDest();
    macAddressInd->setSrcAddress(header->getSrc());
    macAddressInd->setDestAddress(destAddr);

    if (par("promiscuous").boolValue())
        return true;
    if (destAddr.isBroadcast())
        return true;
    if (destAddr.isMulticast()) {
        // TODO check it in the multicast address list
        return true;
    }
    if (interfaceEntry != nullptr && destAddr == interfaceEntry->getMacAddress())
        return true;

    //TODO
    // should push back header before emit packetdrop signal ??? or else reset packet iterators ???
    // emit packetdrop: NOT_ADDRESSED_TO_US (base class currently emits OTHER_PACKET_DROP)

    return false;
}

} // namespace inet

