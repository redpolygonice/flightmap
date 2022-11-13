#include "commbuilder.h"
#include "common/log.h"

#include "comms/tcpcomm.h"
#include "comms/udpccomm.h"
#include "comms/udpscomm.h"
#include "comms/uartcomm.h"
#include "comms/mockcomm.h"

namespace comms
{

CommBuilder::CommBuilder()
{
}

CommunicationPtr CommBuilder::create(const common::ConnectParams &params)
{
	CommunicationPtr comm = nullptr;

	// First create communication
	if (params.protocol == "TCP")
	{
		comm = comms::TcpComm::create(params);
	}
	else if (params.protocol == "UDPC")
	{
		comm = comms::UdpcComm::create(params);
	}
	else if (params.protocol == "UDPS")
	{
		comm = comms::UdpsComm::create(params);
	}
	else if (params.protocol == "Mock")
	{
		comm = comms::MockComm::create(params);
	}
	else if (params.protocol.find(UART_NAME) == 0 || params.protocol.find(PIXHAWK_NAME) == 0)
	{
		comm = comms::UartComm::create(params);
	}
	else
	{
		LOGE("[CommBuilder] Can't create connection, wrong params!");
		return nullptr;
	}

	// Try to open device
	if (!comm->open())
	{
		LOGE("[CommBuilder] Can't open connection!");
		comm.reset();
		return nullptr;
	}

	return comm;
}

}
