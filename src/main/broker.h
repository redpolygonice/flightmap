#ifndef BROKER_H
#define BROKER_H

#include "common/types.h"
#include "common/singleton.h"
#include "devices/devicemanager.h"
#include "devices/connection.h"
#include "data/telebox.h"
#include "common/devicetypes.h"
#include "data/srtm.h"
#include "data/icoordinate.h"
#include "common/anymap.h"

namespace core
{

typedef std::function<void(const data::CoordinateList &)> ReadMissionFunction;
typedef std::function<void(bool)> ConnectFunction;

// The main core class
class Broker : public Singleton<Broker>
{
	friend class Singleton<Broker>;

private:
	device::DeviceManagerPtr _deviceManager;
	data::Srtm _srtm;
	string _imagePath;

	// Channels overrides
	std::map<int, uint16_t> _channels;
	const uint16_t channelLow = 1100;
	const uint16_t channelHigh = 1900;
	const uint16_t defaultChannel = 1500;

	// Key press control
	const int _timeInterval = 100;
	std::chrono::high_resolution_clock::time_point _timePoint;

private:
	Broker();

private:
	bool PressControl();
	void ClearChannels() { 	for (auto& [key, value] : _channels) _channels[key] = 0; }
	void DefaultChannels() { for (auto& [key, value] : _channels) _channels[key] = defaultChannel; }
	void CreateImagePath();

public:
	~Broker();

public:
	void Stop();
	bool IsConnected() const;
	bool Connect(const common::ConnectParams &params);
	bool ConnectAsync(const common::ConnectParams &params, const ConnectFunction &func);
	void Disconnect();
	void Disconnect(int number);

	device::ConnectionPtr ActiveConnection() const { return _deviceManager->Active(); }
	device::ConnectionPtr FindConnection(const common::ConnectParams &params) const;
	device::DevicePtr ActiveDevice() const { return ActiveConnection() == nullptr ? nullptr : ActiveConnection()->GetDevice(); }
	data::TeleBoxPtr Telebox() const { return ActiveConnection() == nullptr ? nullptr : ActiveConnection()->GetDevice()->Telebox(); }
	void SetAcive(int number);
	float GetAltitude(float lat, float lon) { return _srtm.GetAltitude(lat, lon); }
	static float ComputeAlt(float lat, float lon, bool check, int type, int value);
	void SetMapZoom(double zoom) {}
	string ImageLocation() const { return _imagePath; }

	// Mission
	bool CreateMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params);
	bool ClearMission();
	bool WriteMission(const data::CoordinateList &points, const common::AnyMap &params);
	bool WriteMissionAsync(const data::CoordinateList &points, const common::AnyMap &params);
	bool ReadMission(data::CoordinateList &points);
	bool ReadMissionAsync(const ReadMissionFunction &func);
	bool SaveMissionToFile(const string &fileName, const data::CoordinateList &points, const common::AnyMap &params);
	bool LoadMissionFromFile(const string &fileName, data::CoordinateList &points, common::AnyMap &params);

	// Device commands
	void ProcessKeyDown(KeyCode key);
	void ProcessKeyUp(KeyCode key);
	void StartMission();
	void SetHome(const data::CoordinatePtr &point);
	void SetPosition(const data::CoordinatePtr &point);
	void SetMode(uint32_t mode);
	void Arm(bool arm);
	void Loiter();
	void rtl();
	void Auto();
	void WriteChannel(int index, uint16_t rc);
	void SetDefaultChannel(int index);
	void SetDefaultChannels();
	void SendServoCommand(float index, float ch);
	void SendCommand(uint32_t cmd, float param1 = 0, float param2 = 0, float param3 = 0, float param4 = 0, float param5 = 0,
					 float param6 = 0, float param7 = 0);

	// Change position
	void MoveUp(bool stop = false);
	void MoveDown(bool stop = false);
	void MoveLeft(bool stop = false);
	void MoveRight(bool stop = false);
	void MoveForward(bool stop = false);
	void MoveBack(bool stop = false);
	void TurnLeft(bool stop = false);
	void TurnRight(bool stop = false);

	// Camera
	void StartCamera();
	void StopCamera();
};

}

#endif // BROKER_H
