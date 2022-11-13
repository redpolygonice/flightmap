#ifndef BROKER_H
#define BROKER_H

#include "common/types.h"
#include "common/singleton.h"
#include "devices/connectionmanager.h"
#include "devices/connection.h"
#include "common/devicetypes.h"
#include "data/srtm.h"
#include "data/coordinate.h"
#include "common/anymap.h"

namespace core
{

// The main core class
class Broker : public Singleton<Broker>
{
	friend class Singleton<Broker>;

private:
	bool _connected;
	device::ConnectionManagerPtr _connectManager;
	data::Srtm _srtm;
	std::future<void> _future;

	std::map<int, uint16_t> _channels;
	uint16_t channelLow = 1100;
	uint16_t channelHigh = 1900;

private:
	Broker();

public:
	~Broker();

public:
	void stop();
	bool isConnected() const { return _connected; }
	bool connect(const common::ConnectParams &params);
	void disconnect();

	device::ConnectionPtr activeConnection() const { return _connectManager->active(); }
	device::DevicePtr activeDevice() const { return activeConnection() == nullptr ? nullptr : activeConnection()->device(); }
	data::TeleBoxPtr telebox() const { return activeConnection() == nullptr ? nullptr : activeConnection()->device()->telebox(); }
	float getAltitude(float lat, float lon) { return _srtm.getAltitude(lat, lon); }

	// Mission
	bool createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params);
	bool clearMission();
	bool writeMission(const data::CoordinateList &points, const common::AnyMap &params);
	bool readMission(data::CoordinateList &points);

	// Device commands
	void startMission();
	void setHome(const data::CoordinatePtr &point);
	void setPosition(const data::CoordinatePtr &point);
	void setMode(uint32_t mode);
	void armDisarm(bool arm);
	void loiter();
	void rtl();
	void autom();
	void writeChannel(int index, uint16_t rc);
	void setDefaultChannel(int index);
	void setDefaultChannels();
	void sendServoCommand(float index, float ch);
	void sendCommand(uint32_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0, float param5 = 0,
					 float param6 = 0, float param7 = 0);
};

}

#endif // BROKER_H
