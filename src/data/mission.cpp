#include "mission.h"
#include "common/log.h"
#include "common/common.h"
#include "data/grid.h"
#include "qtcoordinate.h"

#include <mavlink/ardupilotmega/ardupilotmega.h>

namespace data
{

Mission::Mission(const device::DevicePtr &device)
	: _grid(nullptr)
	, _device(device)
{
	_currentItem.point = QtCoordinate::create();
}

Mission::~Mission()
{
}

bool Mission::createPoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params)
{
	if (_grid == nullptr)
		_grid = Grid::create();

	return _grid->create(markers, mission, params);
}

bool Mission::processMessage(const mavlink_message_t &message)
{
	_sequence = message.seq;
	_targetSysid = message.sysid;
	_targetCompid = message.compid;

	switch (message.msgid) {
	case MAVLINK_MSG_ID_MISSION_COUNT:
	{
		LOGD("MAVLINK_MSG_ID_MISSION_COUNT");
		mavlink_mission_count_t data;
		mavlink_msg_mission_count_decode(&message, &data);
		std::unique_lock<std::mutex> lock(_mutex);
		_missionCount = data.count;
		_missionCond.notify_all();
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_REQUEST:
	{
		mavlink_mission_request_t data;
		mavlink_msg_mission_request_decode(&message, &data);
		_missionSequence = data.seq;
		LOGD("MAVLINK_MSG_ID_MISSION_REQUEST _missionSequence " << _missionSequence);
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_REQUEST_INT:
	{
		mavlink_mission_request_int_t data;
		mavlink_msg_mission_request_int_decode(&message, &data);
		_missionSequence = data.seq;
		LOGD("MAVLINK_MSG_ID_MISSION_REQUEST_INT _missionSequence " << _missionSequence);
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_ITEM:
	{
		LOGD("MAVLINK_MSG_ID_MISSION_ITEM");
		mavlink_mission_item_t data;
		mavlink_msg_mission_item_decode(&message, &data);
		std::unique_lock<std::mutex> lock(_mutex);
		_currentItem.cmd = data.command;
		_currentItem.param1 = data.param1;
		_currentItem.param2 = data.param2;
		_currentItem.param3 = data.param3;
		_currentItem.param4 = data.param4;
		_currentItem.point->setLat(data.x);
		_currentItem.point->setLon(data.y);
		_currentItem.point->setAlt(data.z);
		_isMissionItem = true;
		_missionCond.notify_all();
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_ITEM_INT:
	{
		LOGD("MAVLINK_MSG_ID_MISSION_ITEM_INT");
		mavlink_mission_item_int_t data;
		mavlink_msg_mission_item_int_decode(&message, &data);
		std::unique_lock<std::mutex> lock(_mutex);
		_currentItem.cmd = data.command;
		_currentItem.param1 = data.param1;
		_currentItem.param2 = data.param2;
		_currentItem.param3 = data.param3;
		_currentItem.param4 = data.param4;
		_currentItem.point->setLat((double)(data.x / 1.0e7));
		_currentItem.point->setLon((double)(data.y / 1.0e7));
		_currentItem.point->setAlt(data.z);
		_isMissionItem = true;
		_missionCond.notify_all();
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_ACK:
	{
		LOGD("MAVLINK_MSG_ID_MISSION_ACK");
		mavlink_mission_ack_t data;
		mavlink_msg_mission_ack_decode(&message, &data);
		std::unique_lock<std::mutex> lock(_mutex);
		_isMissionResult = true;
		_missionResult = (MAV_MISSION_RESULT)data.type;
		_missionCond.notify_all();
		return true;
	}
	}

	return false;
}

void Mission::addCommand(MissionItemList &items, int cmd, float p1, float p2, float p3, float p4, const CoordinatePtr &point)
{
	MissionItem item = { cmd, p1, p2, p3, p4, point };
	items.push_back(item);
}

void Mission::createCommands(const CoordinateList &points, MissionItemList &items)
{
	// Take off
	addCommand(items, MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, points[0]);
	addCommand(items, MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, points[0]);

	// Waypoints
	for (int i = 1; i < points.size(); ++i)
		addCommand(items, MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, points[i]);

	// Return to launch
	addCommand(items, MAV_CMD_NAV_RETURN_TO_LAUNCH, 0, 0, 0, 0, points[0]);
}

void Mission::clear()
{
	mavlink_message_t data;
	mavlink_mission_clear_all_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	mavlink_msg_mission_clear_all_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_clear_all_t error!");
}

void Mission::write(const CoordinateList &points, const common::AnyMap &params)
{
	data::MissionItemList items;
	createCommands(points, items);

	common::AnyMap missionParams = params;
	common::AltType altType = static_cast<common::AltType>(missionParams.get<int>("altType"));
	MAV_FRAME frame = MAV_FRAME_GLOBAL;

	if (altType == common::AltType::Absolute)
		frame = MAV_FRAME_GLOBAL;
	else if (altType == common::AltType::Terrain)
		frame = MAV_FRAME_GLOBAL_TERRAIN_ALT;
	else if (altType == common::AltType::Relative)
		frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;

	LOG("Writing mission ..");

	LOGD("Clear previous mission ..");
	clear();
	common::sleep(100);

	LOGD("Setting mission count ..");
	setMissionCount(items.size());
	common::sleep(100);

	bool ok = true;
	for (int i = 0; i < items.size(); ++i)
	{
		LOGD("Writing point " << (i + 1) << " ..");
		int result = addMissionItem(items[i], frame, i, 1);
		if (result == MAV_MISSION_ACCEPTED)
		{
			LOG("Point " << (i + 1) << " is written");
			LOGD("Point " << (i + 1) << ", cmd: " << items[i].cmd << ", lat: " << items[i].point->lat() << ", lon: " << items[i].point->lon() << ", alt: " << items[i].point->alt());
		}
		else
		{
			LOG("Error writing point " << (i + 1) << ", code " << result);
			ok = false;
			break;
		}
	}

	if (ok)
	{
		//setMissionAck();
		LOG("Mission is written to device!");
	}
	else
	{
		LOG("Mission is not written!");
	}
}

void Mission::read(CoordinateList &points)
{
	LOG("Getting mission ..");
	points.clear();
	int count = getMissionCount();
	if (count > 0)
	{
		LOG("Mission points count: " << count);
		for (int i = 0; i < count; ++i)
		{
			LOG("Getting point " << (i + 1));
			data::MissionItem item;
			if (getMissionItem(i, item))
			{
				LOGD("Got point " << (i + 1) << ", cmd: " << item.cmd << ", lat: " << item.point->lat() << ", lon: " << item.point->lon() << ", alt: " << item.point->alt());
				points.push_back(item.point);
			}
			else
			{
				LOG("Getting point " << (i + 1) << " error!");
				return;
			}
		}

		//setMissionAck();
		LOG("Mission is restored!");
	}
	else
		LOG("Mission is not restored, waypoints empty!");
}

void Mission::setCapabilities(uint64_t capabilities)
{
	_missionIntCap = (capabilities & (uint64_t)MAV_PROTOCOL_CAPABILITY_MISSION_INT) > 0;
}

int Mission::addMissionItem(const MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue)
{
	if (_missionIntCap)
		createMissionItemInt(item, frame, sequence, 1);
	else
		createMissionItem(item, frame, sequence, 1);

	std::unique_lock<std::mutex> lock(_mutex);
	_missionCond.wait_for(lock, std::chrono::milliseconds(500));

	if (_isMissionResult)
	{
		_isMissionResult = false;
		return _missionResult;
	}

	return _missionResult;
}

void Mission::createMissionItem(const MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue)
{
	mavlink_message_t data;
	mavlink_mission_item_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.command = item.cmd;
	message.frame = frame;
	message.current = 0;
	message.seq = sequence;
	message.autocontinue = autocontinue;
	message.mission_type = 0;
	message.param1 = 0;
	message.param2 = 0;
	message.param3 = 0;
	message.param4 = 0;
	message.x = item.point->lat();
	message.y = item.point->lon();
	message.z = item.point->alt();
	mavlink_msg_mission_item_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_item_t error!");
}

void Mission::createMissionItemInt(const MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue)
{
	mavlink_message_t data;
	mavlink_mission_item_int_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.command = item.cmd;
	message.frame = frame;
	message.current = 0;
	message.seq = sequence;
	message.autocontinue = autocontinue;
	message.mission_type = 0;
	message.param1 = 0;
	message.param2 = 0;
	message.param3 = 0;
	message.param4 = 0;
	message.x = (int)(item.point->lat() * 1.0e7);
	message.y = (int)(item.point->lon() * 1.0e7);
	message.z = item.point->alt();
	mavlink_msg_mission_item_int_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_item_t error!");
}

void Mission::setMissionAck()
{
	mavlink_message_t data;
	mavlink_mission_ack_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.type = 0;
	mavlink_msg_mission_ack_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_ack_t error!");
}

void Mission::setMissionCount(uint16_t count)
{
	mavlink_message_t data;
	mavlink_mission_count_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.count = count;
	mavlink_msg_mission_count_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_count_t error!");
}

uint16_t Mission::getMissionCount()
{
	_missionCount = 0;
	mavlink_message_t data;
	mavlink_mission_request_list_t message;
	memset(&message, 0, sizeof(message));
	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	mavlink_msg_mission_request_list_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_request_list_t error!");

	std::unique_lock<std::mutex> lock(_mutex);
	_missionCond.wait_for(lock, std::chrono::milliseconds(1000));

	if (_missionCount > 0)
		return _missionCount;

	return 0;
}

bool Mission::getMissionItem(int index, MissionItem &item)
{
	if (_missionIntCap)
		requestMissionInt(index);
	else
		requestMission(index);

	std::unique_lock<std::mutex> lock(_mutex);
	_missionCond.wait_for(lock, std::chrono::milliseconds(1000));

	if (_isMissionItem)
	{
		_isMissionItem = false;
		item = _currentItem;;
		return true;
	}

	return false;
}

void Mission::requestMission(int index)
{
	mavlink_message_t data;
	mavlink_mission_request_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.seq = (uint16_t)index;
	mavlink_msg_mission_request_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_request_t error!");
}

void Mission::requestMissionInt(int index)
{
	mavlink_message_t data;
	mavlink_mission_request_int_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.seq = (uint16_t)index;
	mavlink_msg_mission_request_int_encode(_systemId, _componentId, &data, &message);

	if (!_device->sendMessage(data))
		LOGE("Message mavlink_mission_request_t error!");
}

}
