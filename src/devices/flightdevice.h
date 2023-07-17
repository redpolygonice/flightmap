#ifndef BASEDEVICE_H
#define BASEDEVICE_H

#include "common/types.h"
#include "devices/idevice.h"
#include "comms/icommunication.h"
#include "data/telebox.h"
#include "data/imission.h"

namespace device
{

static std::vector<int> missionTypes = { MAVLINK_MSG_ID_MISSION_COUNT, MAVLINK_MSG_ID_MISSION_REQUEST,
										 MAVLINK_MSG_ID_MISSION_REQUEST_INT, MAVLINK_MSG_ID_MISSION_ITEM,
										 MAVLINK_MSG_ID_MISSION_ITEM_INT, MAVLINK_MSG_ID_MISSION_ACK,
										 MAVLINK_MSG_ID_MISSION_REQUEST_LIST, MAVLINK_MSG_ID_MISSION_SET_CURRENT };

// Base device
class FlightDevice : public IDevice, public std::enable_shared_from_this<FlightDevice>
{
public:
	enum FlightModes : uint32_t
	{
		ModeStabilize = 0,
		ModeAcro = 1,
		ModeAltHold = 2,
		ModeAuto = 3,
		ModeGuided = 4,
		ModeLoiter = 5,
		ModeRtl = 6,
		ModeCircle = 7,
		ModeLand = 9,
		ModeDrift = 11,
		ModeSport = 13,
		ModeFlip = 14,
		ModeAutotune = 15,
		ModePoshold = 16,
		ModeBrake = 17,
		ModeThrow = 18,
	};

protected:
	comms::CommunicationPtr _comm;
	data::TeleBoxPtr _telebox;
	data::MissionPtr _mission;
	common::DeviceType _type;

	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

public:
	FlightDevice(const comms::CommunicationPtr &comm) : _comm(comm), _telebox(nullptr), _mission(nullptr) {}
	virtual ~FlightDevice() {}

public:
	comms::CommunicationPtr Communication() const override { return _comm; }
	data::TeleBoxPtr Telebox() const override { return _telebox; }
	data::MissionPtr Mission() const override { return _mission; }
	common::DeviceType Type() const override { return _type; }
	virtual string Name() const override { return string(); }
	virtual bool Start() override { return true; }
	virtual void Stop() override {}
	virtual bool WaitHeartbeat() override { return true; }
	virtual bool Send(const mavlink_message_t &message) override;
	virtual bool CreateMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params) override;
	virtual void ClearMission() override;
	virtual void WriteMission(const data::CoordinateList &points, const common::AnyMap &params) override;
	virtual void ReadMission(data::CoordinateList &points) override;
	virtual void SendCommand(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float param5 = 0, float param6 = 0, float param7 = 0) override;
	virtual void SendCommandInt(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
								float x = 0, float y = 0, float z = 0) override;
	virtual void RcChannelsOverride(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4) override;
	virtual void SetMode(uint32_t mode) override;
	virtual void SetHome(float lat, float lon, float alt) override;
	virtual void SetPosition(double lat, double lon, double alt) override;
};

}

#endif // BASEDEVICE_H
