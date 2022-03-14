#include "udpccomm.h"

namespace comms
{

UdpcComm::UdpcComm(const common::ConnectParams &params)
	: ICommunication(params)
{
}

UdpcComm::~UdpcComm()
{
}

bool UdpcComm::open(const std::string &host, unsigned short port)
{
	return false;
}

void UdpcComm::close()
{
}

void UdpcComm::start(const ReadFunction &callback)
{
}

bool UdpcComm::write(const unsigned char *buffer, size_t size)
{
	return false;
}

}
