#ifndef IDEVICE_H
#define IDEVICE_H

#include "common/types.h"
#include "comms/icommunication.h"
#include "data/telebox.h"
#include "data/imission.h"
#include "data/icoordinate.h"
#include "common/anymap.h"
#include "onboard/camera.h"

#include <climits>

namespace device
{

// Device interface
class IDevice
{
public:
	IDevice() {}
	virtual ~IDevice() {}

public:
	virtual comms::CommunicationPtr Communication() const { return nullptr; }
	virtual data::TeleBoxPtr Telebox() const { return nullptr; }
	virtual data::MissionPtr Mission() const { return nullptr; }
	virtual common::DeviceType Type() const { return common::DeviceType::Mock; }
	virtual onboard::CameraPtr Camera() const { return nullptr; }
	virtual string Name() const { return string(); }
	virtual bool Start() = 0;
	virtual void Stop() = 0;
	virtual void SetActive() {}
	virtual bool WaitHeartbeat() { return true; }
	virtual bool Send(const mavlink_message_t &message) { return false; }
	virtual void SetCapabilities(uint64_t capabilities) {}
	virtual bool CreateMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params) = 0;
	virtual void ClearMission() = 0;
	virtual void WriteMission(const data::CoordinateList &points, const common::AnyMap &params) = 0;
	virtual void ReadMission(data::CoordinateList &points) = 0;
	virtual void SendCommand(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
							 float param5 = 0, float param6 = 0, float param7 = 0) = 0;
	virtual void SendCommandInt(uint16_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0,
								float x = 0, float y = 0, float z = 0) = 0;
	virtual void RcChannelsOverride(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4) {}
	virtual void SetPosition(double lat, double lon, double alt) {}
	virtual void SetMode(uint32_t mode) {}
	virtual void SetHome(float lat, float lon, float alt) {}
	virtual void StartCamera() = 0;
	virtual void StopCamera() = 0;
	virtual void SetCamWork(bool arg = true) = 0;
};

typedef std::shared_ptr<IDevice> DevicePtr;

}

#endif // IDEVICE_H
