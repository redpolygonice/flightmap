#include "broker.h"
#include "common/log.h"
#include "common/common.h"

#include "devices/idevice.h"
#include "comms/icommunication.h"
#include "comms/commbuilder.h"
#include "devices/pixhawk.h"
#include "devices/raspi.h"
#include "devices/mock.h"

#include <mavlink/common/mavlink.h>
#include <mavlink/ardupilotmega/ardupilotmega.h>

namespace core
{

#define CHECK_DEVICE()						\
if (!_connected)							\
	return false;							\
device::DevicePtr device = activeDevice();	\
if (device == nullptr)						\
{											\
	LOGW("There is no connected device!");	\
	return false;							\
}

Broker::Broker()
	: _connected(false)
	, _connectManager(nullptr)
{
	_connectManager = device::ConnectionManager::create();
}

Broker::~Broker()
{
}

void Broker::stop()
{
	_connectManager->removeAll();
	_connected = false;
}

bool Broker::connect(const common::ConnectParams &params)
{
	_connected = false;

	// First create communication and check it
	comms::CommBuilder commBuilder;
	comms::CommunicationPtr comm = commBuilder.create(params);
	if (comm == nullptr)
	{
		LOGE("[Broker] Can't create communication!");
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
	else if (params.device == "Mock")
	{
		device = device::Mock::create(comm);
	}
	else
	{
		LOGE("[Broker] Can't create device, wrong params!");
		return false;
	}

	// Create connection object
	_connectManager->add(device);
	device->start();

	_connected = true;
	return true;
}

void Broker::disconnect()
{
	if (!_connected)
		return;

	string deviceName = _connectManager->active()->device()->name();
	_connectManager->active()->device()->stop();
	_connectManager->remove(_connectManager->active()->device());

	LOG("[Broker] Device " << deviceName << " disconnected");
	_connected = false;
}

bool Broker::createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params)
{
	CHECK_DEVICE()
	return device->createMission(points, mission, params);
}

bool Broker::clearMission()
{
	CHECK_DEVICE()
	device->clearMission();
	return true;
}

bool Broker::writeMission(const data::CoordinateList &points, const common::AnyMap &params)
{
	CHECK_DEVICE()
	_future = std::async(std::launch::async, [&]() { device->writeMission(points, params); });
	_future.wait();
	return true;
}

bool Broker::readMission(data::CoordinateList &points)
{
	CHECK_DEVICE()
	_future = std::async(std::launch::async, [&]() { device->readMission(points); });
	_future.wait();
	return true;
}

void Broker::startMission()
{
	if (activeDevice() == nullptr)
		return;

	// Loiter
	loiter();
	common::sleep(1000);

	// Arm
	armDisarm(true);
	common::sleep(2000);

	// Set channels
	_channels[1] = 1500;
	_channels[2] = 1500;
	_channels[3] = 1500;
	_channels[4] = 1500;

	activeDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
	common::sleep(500);
}

void Broker::setHome(const data::CoordinatePtr &point)
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->setHome(point->lat(), point->lon(), point->alt());
}

void Broker::setPosition(const data::CoordinatePtr &point)
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->setMode(device::IDevice::FlightModes::MODE_GUIDED);
	activeDevice()->setPosition(point->lat(), point->lon(), point->alt());
}

void Broker::setMode(uint32_t mode)
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->setMode(mode);
}

void Broker::armDisarm(bool arm)
{
	if (activeDevice() == nullptr)
		return;

	float param1 = (arm == true ? 1 : 0);
	float param2 = (arm == true ? 2989.0f : 21196.0f);

	activeDevice()->sendCommand(MAV_CMD_COMPONENT_ARM_DISARM, param1, param2);
}

void Broker::loiter()
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->sendCommand(MAV_CMD_NAV_LOITER_UNLIM, 1);
}

void Broker::rtl()
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->sendCommand(MAV_CMD_NAV_RETURN_TO_LAUNCH, 1);
}

void Broker::autom()
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->setMode(device::IDevice::FlightModes::MODE_AUTO);
}

void Broker::writeChannel(int index, uint16_t rc)
{
	if (activeDevice() == nullptr)
		return;

	if (index < 1 || index > 4)
		return;

	_channels[index] = rc;
	activeDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
}

void Broker::setDefaultChannel(int index)
{
	if (activeDevice() == nullptr)
		return;

	if (index < 1 || index > 4)
		return;

	_channels[index] = 1500;
	activeDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
}

void Broker::setDefaultChannels()
{
	if (activeDevice() == nullptr)
		return;

	_channels[1] = 1500;
	_channels[2] = 1500;
	_channels[3] = 1500;
	_channels[4] = 1500;
	activeDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
}

void Broker::sendServoCommand(float index, float ch)
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->sendCommand(MAV_CMD_DO_SET_SERVO, index, ch);
}

void Broker::sendCommand(uint32_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
	if (activeDevice() == nullptr)
		return;

	activeDevice()->sendCommand(cmd, param1, param2, param3, param4, param5, param6, param7);
}

}
