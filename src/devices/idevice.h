#ifndef IDEVICE_H
#define IDEVICE_H

#include "common/types.h"
#include "comms/icommunication.h"
#include "data/telebox.h"
#include "data/imission.h"
#include "common/devicetypes.h"

namespace device
{

// Device interface
class IDevice
{
protected:
	comms::CommunicationPtr _comm;
	data::TeleBoxPtr _telebox;
	data::MissionPtr _mission;
	common::DeviceType _type;

public:
	IDevice(const comms::CommunicationPtr &comm) : _comm(comm), _mission(nullptr) {}
	virtual ~IDevice() {}

public:
	comms::CommunicationPtr communication() const { return _comm; }
	data::TeleBoxPtr telebox() const { return _telebox; }

	common::DeviceType type() const { return _type; }
	virtual string name() const { return string(); }
	virtual bool start() = 0;
	virtual void stop() = 0;
	virtual void sendCommand(uint32_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float param5 = 0, float param6 = 0, float param7 = 0) = 0;
};

typedef std::shared_ptr<IDevice> DevicePtr;

}

#endif // IDEVICE_H
