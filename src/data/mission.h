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
	std::condition_variable _missionCond;

	uint8_t _systemId = 255;
	uint8_t _componentId = 0;
	uint8_t _targetSysid = 0;
	uint8_t _targetCompid = 0;
	uint8_t _sequence = 0;

	std::atomic_bool _isMissionItem = false;
	std::atomic_bool _isMissionResult = false;
	std::atomic_uint16_t _missionCount = 0;
	MAV_MISSION_RESULT _missionResult = MAV_MISSION_ACCEPTED;
	std::atomic_uint16_t _missionSequence = 0;
	bool _missionIntCap = false;
	MissionItem _currentItem;

public:
	Mission(const device::DevicePtr &device);
	~Mission();

public:
	static MissionPtr create(const device::DevicePtr &device) { return std::make_shared<Mission>(device); }
	bool processMessage(const mavlink_message_t &message) override;
	bool createPoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) override;
	void createCommands(const CoordinateList &points, MissionItemList &items) override;
	void clear() override;
	void write(const data::CoordinateList &points, const common::AnyMap &params) override;
	void read(data::CoordinateList &points) override;
	void setCapabilities(uint64_t capabilities) override;

private:
	void addCommand(MissionItemList &items, int cmd, float p1, float p2, float p3, float p4, const CoordinatePtr &point);
	int addMissionItem(const data::MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue);
	bool getMissionItem(int index, data::MissionItem &item);
	void createMissionItem(const data::MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue);
	void createMissionItemInt(const data::MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue);
	void setMissionAck();
	void setMissionCount(uint16_t count);
	uint16_t getMissionCount();
	void requestMission(int index);
	void requestMissionInt(int index);
};

}

#endif // MISSION_H
