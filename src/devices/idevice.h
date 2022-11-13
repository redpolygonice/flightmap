#ifndef IDEVICE_H
#define IDEVICE_H

#include "common/types.h"
#include "comms/icommunication.h"
#include "data/telebox.h"
#include "data/imission.h"
#include "common/devicetypes.h"
#include "data/missionitem.h"
#include <climits>

namespace device
{

// Device interface
class IDevice
{
public:
	enum FlightModes : uint32_t
	{
		MODE_STABILIZE = 0,
		MODE_ACRO = 1,
		MODE_ALT_HOLD = 2,
		MODE_AUTO = 3,
		MODE_GUIDED = 4,
		MODE_LOITER = 5,
		MODE_RTL = 6,
		MODE_CIRCLE = 7,
		MODE_LAND = 9,
		MODE_DRIFT = 11,
		MODE_SPORT = 13,
		MODE_FLIP = 14,
		MODE_AUTOTUNE = 15,
		MODE_POSHOLD = 16,
		MODE_BRAKE = 17,
		MODE_THROW = 18,
	};

protected:
	comms::CommunicationPtr _comm;
	data::TeleBoxPtr _telebox;
	data::MissionPtr _mission;
	common::DeviceType _type;

public:
	IDevice(const comms::CommunicationPtr &comm) : _comm(comm), _telebox(nullptr), _mission(nullptr) {}
	virtual ~IDevice() {}

public:
	comms::CommunicationPtr communication() const { return _comm; }
	data::TeleBoxPtr telebox() const { return _telebox; }
	data::MissionPtr mission() const { return _mission; }

	common::DeviceType type() const { return _type; }
	virtual string name() const { return string(); }
	virtual bool start() = 0;
	virtual void stop() = 0;
	virtual bool sendMessage(mavlink_message_t &message) { return false; }
	virtual void setCapabilities(uint64_t capabilities) {}
	virtual void sendCommand(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float param5 = 0, float param6 = 0, float param7 = 0) = 0;
	virtual void sendCommandInt(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float x = 0, float y = 0, float z = 0) = 0;
	virtual bool createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params) = 0;
	virtual void clearMission() = 0;
	virtual void writeMission(const data::CoordinateList &points, const common::AnyMap &params) = 0;
	virtual void readMission(data::CoordinateList &points) = 0;

	virtual void RcChannelsOverride(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4, uint16_t ch5 = UINT16_MAX,
		uint16_t ch6 = UINT16_MAX) {}
	virtual void setPosition(double lat, double lon, double alt) {}
	virtual void setMode(uint32_t mode) {}
	virtual void setHome(float lat, float lon, float alt) {}
};

typedef std::shared_ptr<IDevice> DevicePtr;

}

#endif // IDEVICE_H
