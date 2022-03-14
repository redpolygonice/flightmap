#include "tcpcomm.h"

namespace comms
{

TcpComm::TcpComm(const common::ConnectParams &params)
	: ICommunication(params)
{
}

TcpComm::~TcpComm()
{
}

bool TcpComm::open(const std::string &host, unsigned short port)
{
	return false;
}

void TcpComm::close()
{
}

void TcpComm::start(const ReadFunction &callback)
{
}

bool TcpComm::write(const unsigned char *buffer, size_t size)
{
	return false;
}

}
