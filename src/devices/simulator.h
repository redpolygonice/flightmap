#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "common/types.h"
#include "data/icoordinate.h"
#include "common/threadsafequeue.h"
#include "common/anymap.h"

#include <atomic>

namespace device
{

// Device simulator
class Simulator
{
private:
	std::atomic_bool _active;
	ReadFunction _callback;
	std::thread _thread;
	std::thread _processThread;
	std::thread _heartbeatThread;
	std::thread _missionThread;
	std::mutex _mutex;

	// Data queue
	common::ThreadSafeQueue<ByteArrayPtr> _data;
	common::ThreadSafeQueue<ByteArrayPtr> _outdata;

	// Vehicle system
	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

	// Control
	bool _armed = false;
	uint32_t _mode = 0;
	uint16_t _command = 0;
	std::atomic_bool _loiter = false;
	std::atomic_bool _auto = false;

	// Vehicle position
	static constexpr float _rad = M_PI / 180;
	std::atomic<double> _lat = 59.938888;
	std::atomic<double> _lon = 30.315230;
	std::atomic_int32_t _alt = 0;
	std::atomic_int16_t _roll = 0;
	std::atomic_int16_t _pitch = 0;
	std::atomic_int16_t _yaw = 0;
	double _zoom = 18.0;

	const double distStep = 5.0;
	const int maxAlt = 300;
	const int maxRoll = 15;
	const int maxPitch = 10;
	const int maxYaw = 360;
	const double maxZoom = 18.0;
	const double minZoom = 5.0;
	const double zoomCoeff = 0.05;

	// Channels
	const uint16_t channelLow = 1100;
	const uint16_t channelHigh = 1900;
	const uint16_t defaultChannel = 1500;

	// Mission
	static constexpr char missionFile[] = "data/simulator/mission.fmm";
	common::AnyMap _missionParams;
	data::CoordinateList _missionPoints;
	uint16_t _missionCount = 0;
	uint8_t _missionResult = 0;
	MAV_FRAME _missionFrame = MAV_FRAME_GLOBAL;
	uint16_t _missionSeq = 0;

private:
	void StartHeartbeat();
	void ProcessData();
	void UpdateData();
	void Run();
	void RunTest();
	void Send(const mavlink_message_t &message);

	void ProcessMessage(const mavlink_message_t &message);
	void ProcessMissionAck(const mavlink_message_t &message);
	void ProcessMissionClear(const mavlink_message_t &message);
	void ProcessMissionRequestList(const mavlink_message_t &message);
	void ProcessMissionRequest(const mavlink_message_t &message);
	void ProcessMissionRequestInt(const mavlink_message_t &message);
	void ProcessMissionCount(const mavlink_message_t &message);
	void ProcessMissionItem(const mavlink_message_t &message);
	void ProcessMissionItemInt(const mavlink_message_t &message);
	void SendMissionAck(uint8_t result = MAV_MISSION_ACCEPTED);
	void SendMissionCount();
	void SendMissionItem(uint16_t index);
	void SendMissionItemInt(uint16_t index);
	void ProcessRcChanneslOverride(const mavlink_message_t &message);
	void ProcessCommand(const mavlink_message_t &message);
	void ProcessCommandInt(const mavlink_message_t &message);
	void ProcessSetMode(const mavlink_message_t &message);
	void ProcessSetHome(const mavlink_message_t &message);
	void ProcessSetPosition(const mavlink_message_t &message);
	template<typename T> void ProcessCommandArm(const T &data);
	template<typename T> void ProcessCommandLoiter(const T &data);
	template<typename T> void ProcessCommandRtl(const T &data);

	void InitMissionParams();
	void LoadMission();
	void SaveMission();
	void StartMission();
	void StopMission();

public:
	Simulator();
	~Simulator();

public:
	void OnSetActive();
	void Start(const ReadFunction &callback, bool test = false);
	void Stop();
	bool Read(ByteArray &data);
	void Write(const ByteArray &data);
};

}

#endif // SIMULATOR_H
