/*! \file connect_msg.cpp
    \brief Client connect message class. Implements client specific functionality.

    (C) Copyright Intel Corporation 2023. All rights reserved. Intel Confidential.
    Author: Christopher Hall <christopher.s.hall@intel.com>
*/

#include <client/connect_msg.hpp>
#include <common/print.hpp>

using namespace JClkLibClient;
using namespace JClkLibCommon;
using namespace std;

/** @brief Create the ClientConnectMessage object
 *
 * @param msg msg structure to be fill up
 * @param LxContext client run-time transport listener context
 * @return true
 */
MAKE_RXBUFFER_TYPE(ClientConnectMessage::buildMessage)
{
        msg = new ClientConnectMessage();

        return true;
}

/** @brief Add client's CONNECT_MSG type and its builder to transport layer.
 *
 * This function will be called during init to add a map of CONNECT_MSG
 * type and its corresponding buildMessage function.
 *
 * @return true
 */
bool ClientConnectMessage::initMessage()
{
        addMessageType(parseMsgMapElement_t(CONNECT_MSG, buildMessage));
        return true;
}

/** @brief process the reply for connect msg from proxy.
 *
 * This function will be called when the transport layer
 * in client runtime received a CONNECT_MSG type (an echo reply from
 * proxy)
 * In this case, transport layer will rx a buffer in the client associated
 * listening message queue (listening to proxy) and call this function when
 * the enum ID corresponding to the CONNECT_MSG is received.
 * For client-runtime, this will always be the reply, since connect msg
 * is always send first from the client runtime first. The proxy will
 * echo-reply with a different ACK msg.
 *
 * @param LxContext client run-time transport listener context
 * @param TxContext client run-time transport transmitter context
 * @return true
 */
PROCESS_MESSAGE_TYPE(ClientConnectMessage::processMessage)
{
        sessionId_t newSessionId;

        PrintDebug("Processing client connect message (reply)");

	state.set_connected(true);
	state.set_sessionId(this->get_sessionId());

        PrintDebug("Connected with session ID: " + to_string(this->get_sessionId()));

        /*[TODO] client runtime shd return client ID to application - return client ID at least */
        /*[Question] ACK_NONE is necessary ?*/
	this->set_msgAck(ACK_NONE);
        return true;
}
