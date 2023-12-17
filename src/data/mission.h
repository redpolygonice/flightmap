#ifndef MISSION_H
#define MISSION_H

#include "common/types.h"
#include "imission.h"
#include "igrid.h"
#include "devices/idevice.h"

namespace data
{

// Mission implementation
class Mission : public IMission
{
private:
	GridPtr _grid;
	device::DevicePtr _device;
	std::mutex _mutex;
	std::mutex _itemMutex;
	std::condition_variable _waitCond;

	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

	bool _isMissionItem = false;
	bool _isMissionResult = false;
	std::atomic_uint16_t _missionCount = 0;
	MAV_MISSION_RESULT _missionResult = MAV_MISSION_ACCEPTED;
	std::atomic_uint16_t _missionSequence = 0;
	bool _missionIntCap = false;
	MissionItem _currentItem;

public:
	Mission(const device::DevicePtr &device);
	~Mission();

public:
	static MissionPtr Create(const device::DevicePtr &device) { return std::make_shared<Mission>(device); }
	bool ProcessMessage(const mavlink_message_t &message) override;
	bool CreatePoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) override;
	void CreateCommands(const CoordinateList &points, MissionItemList &items) override;
	void Clear() override;
	void Write(const data::CoordinateList &points, const common::AnyMap &params) override;
	void Read(data::CoordinateList &points) override;
	void SetCapabilities(uint64_t capabilities) override;

private:
	void AddCommand(MissionItemList &items, int cmd, float p1, float p2, float p3, float p4, const CoordinatePtr &point);
	int AddMissionItem(const data::MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue);
	bool GetMissionItem(int index, data::MissionItem &item);
	void CreateMissionItem(const data::MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue);
	void CreateMissionItemInt(const data::MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue);
	void SetMissionAck();
	void SetMissionCount(uint16_t count);
	uint16_t GetMissionCount();
	uint16_t GetMissionCount2();
	void RequestMission(uint16_t index);
	void RequestMissionInt(uint16_t index);
	void ProcessMissionAck(const mavlink_message_t &message);
	void ProcessMissionCount(const mavlink_message_t &message);
	void ProcessMissionItem(const mavlink_message_t &message);
	void ProcessMissionItemInt(const mavlink_message_t &message);
	bool IsValidPoint(const data::MissionItem &item) const;
};

}

#endif // MISSION_H
