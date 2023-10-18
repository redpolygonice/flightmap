#include "mission.h"
#include "common/log.h"
#include "common/common.h"
#include "data/grid.h"
#include "qtcoordinate.h"

namespace data
{

Mission::Mission(const device::DevicePtr &device)
	: _grid(nullptr)
	, _device(device)
{
}

Mission::~Mission()
{
}

bool Mission::CreatePoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params)
{
	if (_grid == nullptr)
		_grid = Grid::Create();

	return _grid->Create(markers, mission, params);
}

bool Mission::ProcessMessage(const mavlink_message_t &message)
{
	_sequence = message.seq;
	_targetSysid = message.sysid;
	_targetCompid = message.compid;

	switch (message.msgid) {
	case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
	{
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_SET_CURRENT:
	{
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_REQUEST:
	{
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_REQUEST_INT:
	{
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_COUNT:
	{
		ProcessMissionCount(message);
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_ITEM:
	{
		ProcessMissionItem(message);
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_ITEM_INT:
	{
		ProcessMissionItemInt(message);
		return true;
	}
	case MAVLINK_MSG_ID_MISSION_ACK:
	{
		ProcessMissionAck(message);
		return true;
	}
	}

	return false;
}

void Mission::SetCapabilities(uint64_t capabilities)
{
	_missionIntCap = (capabilities & (uint64_t)MAV_PROTOCOL_CAPABILITY_MISSION_INT) > 0;
}

void Mission::AddCommand(MissionItemList &items, int cmd, float p1, float p2, float p3, float p4, const CoordinatePtr &point)
{
	MissionItem item = { cmd, p1, p2, p3, p4, point->Lat(), point->Lon(), point->Alt() };
	items.push_back(item);
}

void Mission::CreateCommands(const CoordinateList &points, MissionItemList &items)
{
	// Take off
	AddCommand(items, MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, points[0]);
	AddCommand(items, MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, points[0]);

	// Waypoints
	for (int i = 1; i < points.size(); ++i)
	{
		if (points[i]->IsBasePoint())
			continue;

		AddCommand(items, MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, points[i]);
	}

	// Return to launch
	AddCommand(items, MAV_CMD_NAV_RETURN_TO_LAUNCH, 0, 0, 0, 0, points[0]);
}

void Mission::Clear()
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_clear_all_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	mavlink_msg_mission_clear_all_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_clear_all_t error!");
}

void Mission::Write(const CoordinateList &points, const common::AnyMap &params)
{
	data::MissionItemList items;
	CreateCommands(points, items);

	common::AnyMap missionParams = params;
	common::AltType altType = static_cast<common::AltType>(missionParams.Get<int>("altType"));
	MAV_FRAME frame = MAV_FRAME_GLOBAL;

	if (altType == common::AltType::Absolute)
		frame = MAV_FRAME_GLOBAL;
	else if (altType == common::AltType::Terrain)
		frame = MAV_FRAME_GLOBAL_TERRAIN_ALT;
	else if (altType == common::AltType::Relative)
		frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;

	LOG("Writing mission ..");

	LOGD("Clear previous mission ..");
	Clear();
	common::Sleep(100);

	LOGD("Setting mission count ..");
	SetMissionCount(items.size());
	common::Sleep(100);

	bool ok = true;
	for (int i = 0; i < items.size(); ++i)
	{
		LOGD("Writing point " << (i + 1) << " ..");
		int result = AddMissionItem(items[i], frame, i, 1);
		if (result == MAV_MISSION_ACCEPTED)
		{
			LOG("Point " << (i + 1) << " is written");
			LOGD("Point " << (i + 1) << ", cmd: " << items[i].cmd << ", lat: " << items[i].lat << ", lon: " << items[i].lon << ", alt: " << items[i].alt);
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
		LOG("Mission is written to device!");
	}
	else
	{
		LOG("Mission is not written!");
	}
}

void Mission::Read(CoordinateList &points)
{
	LOG("Getting mission ..");

	points.clear();
	bool error = false;
	_isMissionItem = false;
	_isMissionResult = false;
	int count = GetMissionCount();

	if (count > 0)
	{
		LOG("Mission points count: " << count);
		for (int i = 0; i < count; ++i)
		{
			LOG("Getting point " << (i + 1) << " ..");
			data::MissionItem item;
			if (GetMissionItem(i, item))
			{
				if (!IsValidPoint(item))
				{
					LOGW("Point " << (i + 1) << " is not valid!");
					continue;
				}

				LOGD("Got point " << (i + 1) << ", cmd: " << item.cmd << ", lat: " << item.lat << ", lon: " << item.lon << ", alt: " << item.alt);
				CoordinatePtr coordinate = QtCoordinate::create(item.lat, item.lon, item.alt);
				points.push_back(coordinate);
			}
			else
			{
				LOGW("Point " << (i + 1) << " error!");
				error = true;
			}
		}

		if (error)
		{
			LOGW("Mission is restored with errors!");
		}
		else
		{
			LOG("Mission is restored!");
		}
	}
	else
	{
		LOGW("Mission is not restored, there is no waypoints!");
	}
}

int Mission::AddMissionItem(const MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue)
{
	if (_missionIntCap)
		CreateMissionItemInt(item, frame, sequence, 1);
	else
		CreateMissionItem(item, frame, sequence, 1);

	std::unique_lock<std::mutex> lock(_mutex);
	_waitCond.wait_for(lock, std::chrono::milliseconds(500), [this]() {
		return _isMissionResult;
	});

	if (_isMissionResult)
	{
		_isMissionResult = false;
		return _missionResult;
	}

	return _missionResult;
}

void Mission::CreateMissionItem(const MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
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
	message.x = item.lat;
	message.y = item.lon;
	message.z = item.alt;
	mavlink_msg_mission_item_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_item_t error!");
}

void Mission::CreateMissionItemInt(const MissionItem &item, uint8_t frame, uint16_t sequence, uint8_t autocontinue)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
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
	message.x = (int)(item.lat * 1.0e7);
	message.y = (int)(item.lon * 1.0e7);
	message.z = item.alt;
	mavlink_msg_mission_item_int_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_item_t error!");
}

void Mission::SetMissionAck()
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_ack_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.type = 0;
	mavlink_msg_mission_ack_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_ack_t error!");
}

void Mission::SetMissionCount(uint16_t count)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_count_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.count = count;
	mavlink_msg_mission_count_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_count_t error!");
}

uint16_t Mission::GetMissionCount()
{
	_missionCount = 0;
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_request_list_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	mavlink_msg_mission_request_list_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_request_list_t error!");

	std::unique_lock<std::mutex> lock(_mutex);
	_waitCond.wait_for(lock, std::chrono::milliseconds(1000));

	if (_missionCount > 0)
		return _missionCount;

	return 0;
}

uint16_t Mission::GetMissionCount2()
{
	_missionCount = 0;
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_request_list_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	mavlink_msg_mission_request_list_encode(_systemId, _componentId, &data, &message);

	int count = 10;
	while (count-- > 0)
	{
		_device->Send(data);

		std::unique_lock<std::mutex> lock(_mutex);
		_waitCond.wait_for(lock, std::chrono::milliseconds(1000));

		if (_missionCount > 0)
			return _missionCount;
	}

	return 0;
}

bool Mission::GetMissionItem(int index, MissionItem &item)
{
	if (_missionIntCap)
		RequestMissionInt(index);
	else
		RequestMission(index);

	std::unique_lock<std::mutex> lock(_mutex);
	_waitCond.wait_for(lock, std::chrono::milliseconds(2000), [this]() {
		return _isMissionItem;
	});

	if (_isMissionItem)
	{
		item = _currentItem;
		_isMissionItem = false;
		return true;
	}

	return false;
}

void Mission::RequestMission(uint16_t index)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_request_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.seq = index;
	mavlink_msg_mission_request_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_request_t error!");
}

void Mission::RequestMissionInt(uint16_t index)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_mission_request_int_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.seq = index;
	mavlink_msg_mission_request_int_encode(_systemId, _componentId, &data, &message);

	if (!_device->Send(data))
		LOGE("Message mavlink_mission_request_t error!");
}

void Mission::ProcessMissionAck(const mavlink_message_t& message)
{
	std::unique_lock<std::mutex> lock(_mutex);
	mavlink_mission_ack_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_ack_decode(&message, &data);
	_isMissionResult = true;
	_missionResult = (MAV_MISSION_RESULT)data.type;
	_waitCond.notify_one();
}

void Mission::ProcessMissionCount(const mavlink_message_t& message)
{
	std::unique_lock<std::mutex> lock(_mutex);
	mavlink_mission_count_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_count_decode(&message, &data);
	_missionCount = data.count;
	_waitCond.notify_one();
}

void Mission::ProcessMissionItem(const mavlink_message_t& message)
{
	std::unique_lock<std::mutex> lock(_mutex);
	mavlink_mission_item_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_item_decode(&message, &data);
	_currentItem.cmd = data.command;
	_currentItem.param1 = data.param1;
	_currentItem.param2 = data.param2;
	_currentItem.param3 = data.param3;
	_currentItem.param4 = data.param4;
	_currentItem.lat = data.x;
	_currentItem.lon = data.y;
	_currentItem.alt = data.z;
	_isMissionItem = true;
	_waitCond.notify_one();
}

void Mission::ProcessMissionItemInt(const mavlink_message_t& message)
{
	std::unique_lock<std::mutex> lock(_mutex);
	mavlink_mission_item_int_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_item_int_decode(&message, &data);
	_currentItem.cmd = data.command;
	_currentItem.param1 = data.param1;
	_currentItem.param2 = data.param2;
	_currentItem.param3 = data.param3;
	_currentItem.param4 = data.param4;
	_currentItem.lat = (double)(data.x / 1.0e7);
	_currentItem.lon = (double)(data.y / 1.0e7);
	_currentItem.alt = data.z;
	_isMissionItem = true;
	_waitCond.notify_one();
}

bool Mission::IsValidPoint(const MissionItem &item) const
{
	if (item.lat == 0 && item.lon == 0)
		return false;
	return true;
}

}
