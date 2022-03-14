#include "udpscomm.h"

namespace comms
{

UdpsComm::UdpsComm(const common::ConnectParams &params)
	: ICommunication(params)
{
}

UdpsComm::~UdpsComm()
{
}

bool UdpsComm::open(const std::string &host, unsigned short port)
{
	return false;
}

void UdpsComm::close()
{
}

void UdpsComm::start(const ReadFunction &callback)
{
}

bool UdpsComm::write(const unsigned char *buffer, size_t size)
{
	return false;
}

}
