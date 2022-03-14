#include "dog.h"
#include "common/log.h"

#include "devices/idevice.h"
#include "comms/icommunication.h"
#include "comms/commbuilder.h"
#include "devices/pixhawk.h"
#include "devices/raspi.h"

namespace core
{

Dog::Dog()
	: _connected(false)
	, _connections(nullptr)
{
	_connections = data::ConnectionList::create();
}

Dog::~Dog()
{
}

void Dog::stop()
{
	_connections->removeAll();
	_connected = false;
}

bool Dog::connect(const common::ConnectParams &params)
{
	_connected = false;

	// First create communication and check it
	comms::CommBuilder commBuilder;
	comms::CommunicationPtr comm = commBuilder.create(params);
	if (comm == nullptr)
	{
		LOGE("[Dog] Can't create communication!");
		return false;
	}

	// Create device
	device::DevicePtr device = nullptr;
	if (params.device == "Pixhawk")
	{
		device = device::Pixhawk::create(comm);
	}
	else if (params.device == "Raspi")
	{
		device = device::Raspi::create(comm);
	}
	else
	{
		LOGE("[Dog] Can't create device, wrong params!");
		return false;
	}

	// Create connection object
	_connections->add(device);
	device->start();

	_connected = true;
	return true;
}

void Dog::disconnect()
{
	if (!_connected)
		return;

	string deviceName = _connections->active()->device()->name();
	_connections->active()->device()->stop();
	_connections->remove(_connections->active()->device());

	LOG("[Dog] Device " << deviceName << " disconnected");
	_connected = false;
}

}
