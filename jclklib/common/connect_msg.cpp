/*! \file connect_msg.cpp
    \brief Common connect message implementation. Implements common functions and (de-)serialization

    (C) Copyright Intel Corporation 2023. All rights reserved. Intel Confidential.
    Author: Christopher Hall <christopher.s.hall@intel.com>
*/

#include <common/connect_msg.hpp>
#include <common/serialize.hpp>
#include <common/print.hpp>

using namespace JClkLibCommon;
using namespace std;

string CommonConnectMessage::toString()
{
	string name = ExtractClassName(string(__PRETTY_FUNCTION__),string(__FUNCTION__));
	name += "\n";
	name += Message::toString();
	name += "Client ID: " + string((char *)clientId.data()) + "\n";

	return name;
}

PARSE_RXBUFFER_TYPE(CommonConnectMessage::parseBuffer) {
	if(!Message::parseBuffer(LxContext))
		return false;

	if (!PARSE_RX(ARRAY,clientId, LxContext))
		return false;

	return true;
}


BUILD_TXBUFFER_TYPE(CommonConnectMessage::makeBuffer) const
{
	auto ret = Message::makeBuffer(TxContext); 
	if (!ret)
		return ret;

	if (!WRITE_TX(ARRAY,clientId,TxContext))
		return false;

	return true;
}

TRANSMIT_MESSAGE_TYPE(CommonConnectMessage::transmitMessage)
{
	if (!presendMessage(&TxContext))
		return false;

	return TxContext.sendBuffer();
}
