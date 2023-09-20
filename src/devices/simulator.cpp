#include "simulator.h"
#include "common/common.h"
#include "main/broker.h"
#include "data/qtcoordinate.h"
#include "main/proxy.h"
#include "data/grid.h"
#include "common/devicetypes.h"
#include "flightdevice.h"

namespace device
{

#define LOGSIM(text) LOG("[Simulator] " text);

Simulator::Simulator()
	: _active(false)
	, _callback(nullptr)
{
}

Simulator::~Simulator()
{
}

void Simulator::Start(const ReadFunction &callback, bool test)
{
	if (_active)
		return;

	_callback = callback;
	_active = true;
	UpdateData();
	StartHeartbeat();
	_processThread = std::thread([this]() { ProcessData(); });
	_thread = std::thread([this, test]() {
		if (test)
			RunTest();
		else
			Run();
	});

	OnSetActive();
}

void Simulator::Stop()
{
	_active = false;

	if (_thread.joinable())
		_thread.join();

	if (_heartbeatThread.joinable())
		_heartbeatThread.join();

	if (_processThread.joinable())
		_processThread.join();
}

void Simulator::OnSetActive()
{
	core::Proxy()->SetMapZoom(_zoom);
}

bool Simulator::Read(ByteArray &data)
{
	ByteArrayPtr dataPtr;
	if (!_outdata.Pop(std::move(dataPtr)))
		return false;

	data = *dataPtr;
	return true;
}

void Simulator::Write(const ByteArray &data)
{
	ByteArray temp(0, data.size());
	std::copy(data.begin(), data.end(), std::back_inserter(temp));
	_data.Push(std::move(std::make_unique<ByteArray>(temp)));
}

void Simulator::Send(const mavlink_message_t &message)
{
	if (_callback != nullptr)
	{
		const uint8_t *buffer = reinterpret_cast<const uint8_t *>(&message);
		ByteArray data(buffer, buffer + sizeof(message));
		_callback(data);
	}
}

void Simulator::StartHeartbeat()
{
	_heartbeatThread = std::thread([this]()
	{
		mavlink_message_t message;
		memset(&message, 0, sizeof(message));
		mavlink_heartbeat_t heartbeat;
		memset(&heartbeat, 0, sizeof(heartbeat));

		heartbeat.type = MAV_AUTOPILOT_GENERIC;
		heartbeat.system_status = 0;
		heartbeat.custom_mode = 0;
		heartbeat.base_mode = 0;
		heartbeat.autopilot = 0;

		while (_active)
		{
			heartbeat.custom_mode = _mode;
			mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);
			Send(message);
			common::Sleep(1000);
		}
	});
}

void Simulator::ProcessData()
{
	mavlink_message_t message;
	mavlink_status_t status;

	while (_active)
	{
		if (_data.Empty())
		{
			common::Sleep(20);
			continue;
		}

		ByteArrayPtr temp;
		_data.Pop(std::move(temp));

		for (size_t i = 0; i < temp->size(); ++i)
		{
			if (mavlink_parse_char(0, temp->at(i), &message, &status))
			{
				ProcessMessage(message);
			}
		}

		common::Sleep(50);
	}
}

void Simulator::ProcessMessage(const mavlink_message_t &message)
{
	_sequence = message.seq;
	_targetSysid = message.sysid;
	_targetCompid = message.compid;

	if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT)
	{
		mavlink_heartbeat_t heartbeat;
		mavlink_msg_heartbeat_decode(&message, &heartbeat);
	}
	else if (message.msgid == MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE)
	{
		ProcessRcChanneslOverride(message);
		UpdateData();
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_ACK)
	{
		ProcessMissionAck(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_CLEAR_ALL)
	{
		ProcessMissionClear(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_COUNT)
	{
		ProcessMissionCount(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_LIST)
	{
		ProcessMissionRequestList(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST)
	{
		ProcessMissionRequest(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_INT)
	{
		ProcessMissionRequestInt(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_ITEM)
	{
		ProcessMissionItem(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_MISSION_ITEM_INT)
	{
		ProcessMissionItemInt(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_COMMAND_LONG)
	{
		ProcessCommand(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_COMMAND_INT)
	{
		ProcessCommandInt(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_SET_MODE)
	{
		ProcessSetMode(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_SET_HOME_POSITION)
	{
		ProcessSetHome(message);
	}
	else if (message.msgid == MAVLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT)
	{
		ProcessSetPosition(message);
	}
}

void Simulator::ProcessMissionAck(const mavlink_message_t& message)
{
	mavlink_mission_ack_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_ack_decode(&message, &data);
	_missionResult = data.type;
	LOGSIM("MAVLINK_MSG_ID_MISSION_ACK");
}

void Simulator::ProcessMissionClear(const mavlink_message_t& message)
{
	mavlink_mission_clear_all_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_clear_all_decode(&message, &data);
	_missionPoints.clear();
	_missionCount = 0;
	_missionResult = 0;
	_missionFrame = MAV_FRAME_GLOBAL;
	_missionSeq = 0;
	LOGSIM("MAVLINK_MSG_ID_MISSION_CLEAR_ALL");
}

void Simulator::ProcessMissionRequestList(const mavlink_message_t& message)
{
	mavlink_mission_request_list_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_request_list_decode(&message, &data);
	LoadMission();
	SendMissionCount();
	LOGSIM("MAVLINK_MSG_ID_MISSION_REQUEST_LIST");
}

void Simulator::ProcessMissionRequest(const mavlink_message_t& message)
{
	mavlink_mission_request_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_request_decode(&message, &data);
	SendMissionItem(data.seq);
	SendMissionAck();
	LOGSIM("MAVLINK_MSG_ID_MISSION_REQUEST seq=" << data.seq);
}

void Simulator::ProcessMissionRequestInt(const mavlink_message_t& message)
{
	mavlink_mission_request_int_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_request_int_decode(&message, &data);
	SendMissionItemInt(data.seq);
	SendMissionAck();
	LOGSIM("MAVLINK_MSG_ID_MISSION_REQUEST_INT seq=" << data.seq);
}

void Simulator::ProcessMissionCount(const mavlink_message_t& message)
{
	mavlink_mission_count_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_count_decode(&message, &data);
	_missionCount = data.count;
	LOGSIM("MAVLINK_MSG_ID_MISSION_COUNT " << _missionCount);
}

void Simulator::ProcessMissionItem(const mavlink_message_t& message)
{
	mavlink_mission_item_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_item_decode(&message, &data);
	_missionSeq = data.seq;
	data::CoordinatePtr point = data::QtCoordinate::create(data.x, data.y, data.z);
	_missionPoints.push_back(point);
	SendMissionAck();

	if (_missionSeq == _missionCount - 1)
		SaveMission();

	LOGSIM("MAVLINK_MSG_ID_MISSION_ITEM " << point->Lat() << " " << point->Lon() << " " << point->Alt());
}

void Simulator::ProcessMissionItemInt(const mavlink_message_t& message)
{
	mavlink_mission_item_int_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_mission_item_int_decode(&message, &data);
	_missionSeq = data.seq;
	data::CoordinatePtr point = data::QtCoordinate::create((double)(data.x / 1.0e7), (double)(data.y / 1.0e7), data.z);
	_missionPoints.push_back(point);
	SendMissionAck();

	if (_missionSeq == _missionCount - 1)
		SaveMission();

	LOGSIM("MAVLINK_MSG_ID_MISSION_ITEM_INT " << point->Lat() << " " << point->Lon() << " " << point->Alt());
}

void Simulator::SendMissionAck(uint8_t result)
{
	mavlink_message_t message;
	memset(&message, 0, sizeof(message));
	mavlink_mission_ack_t data;
	memset(&data, 0, sizeof(data));

	data.target_component = _targetCompid;
	data.target_system = _targetSysid;
	data.type = result;

	mavlink_msg_mission_ack_encode(_systemId, _componentId, &message, &data);
	Send(message);
}

void Simulator::SendMissionCount()
{
	mavlink_message_t message;
	memset(&message, 0, sizeof(message));
	mavlink_mission_count_t data;
	memset(&data, 0, sizeof(data));

	data.target_component = _targetCompid;
	data.target_system = _targetSysid;
	data.count = _missionPoints.size();

	mavlink_msg_mission_count_encode(_systemId, _componentId, &message, &data);
	Send(message);
}

void Simulator::SendMissionItem(uint16_t index)
{
	if (_missionPoints.size() == 0 || index >= _missionPoints.size())
		return;

	mavlink_message_t message;
	memset(&message, 0, sizeof(message));
	mavlink_mission_item_t data;
	memset(&data, 0, sizeof(data));

	data.target_component = _targetCompid;
	data.target_system = _targetSysid;
	data.command = MAV_CMD_NAV_WAYPOINT;
	data.frame = _missionFrame;
	data.current = 0;
	data.seq = index;
	data.autocontinue = 1;
	//data.mission_type = 0;
	data.param1 = 0;
	data.param2 = 0;
	data.param3 = 0;
	data.param4 = 0;
	data.x = _missionPoints[index]->Lat();
	data.y = _missionPoints[index]->Lon();
	data.z = _missionPoints[index]->Alt();
	mavlink_msg_mission_item_encode(_systemId, _componentId, &message, &data);
	Send(message);
}

void Simulator::SendMissionItemInt(uint16_t index)
{
	if (_missionPoints.size() == 0 || index >= _missionPoints.size())
		return;

	mavlink_message_t message;
	memset(&message, 0, sizeof(message));
	mavlink_mission_item_int_t data;
	memset(&data, 0, sizeof(data));

	data.target_component = _targetCompid;
	data.target_system = _targetSysid;
	data.command = MAV_CMD_NAV_WAYPOINT;
	data.frame = _missionFrame;
	data.current = 0;
	data.seq = index;
	data.autocontinue = 1;
	//data.mission_type = 0;
	data.param1 = 0;
	data.param2 = 0;
	data.param3 = 0;
	data.param4 = 0;
	data.x = (int)(_missionPoints[index]->Lat() * 1.0e7);
	data.y = (int)(_missionPoints[index]->Lat() * 1.0e7);
	data.z = _missionPoints[index]->Lat();
	mavlink_msg_mission_item_int_encode(_systemId, _componentId, &message, &data);
	Send(message);
}

void Simulator::ProcessRcChanneslOverride(const mavlink_message_t &message)
{
	if (!_armed)
	{
		LOGSIM("Not armed!");
		return;
	}

	mavlink_rc_channels_override_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_rc_channels_override_decode(&message, &data);

	// Roll - Move Left and Right
	if (data.chan1_raw > 0)
	{
		bool accepted = false;
		double dist = 0.0;
		double angle = 0.0;
		if (data.chan1_raw == defaultChannel)
		{
			_roll = 0;
		}
		else if (data.chan1_raw < defaultChannel)
		{
			dist += distStep;
			angle = _yaw - 90;
			_roll = -maxRoll;
			accepted = true;
		}
		else
		{
			dist += distStep;
			angle = _yaw + 90;
			_roll = maxRoll;
			accepted = true;
		}

		if (accepted)
		{
			data::CoordinatePtr coord = data::QtCoordinate::create(_lat, _lon);
			coord = coord->AtDistanceAndAzimuth(dist, angle);
			_lat = coord->Lat();
			_lon = coord->Lon();
		}
	}

	// Altitude - Move Up and Down
	if (data.chan2_raw > 0)
	{
		bool accepted = false;
		if (data.chan2_raw == defaultChannel)
		{
		}
		else if (data.chan2_raw < defaultChannel)
		{
			_alt--;
			if (_alt < 0)
				_alt = 0;

			_zoom += zoomCoeff;
			if (_zoom > maxZoom)
				_zoom = maxZoom;

			accepted = true;
		}
		else
		{
			_alt++;
			if (_alt > maxAlt)
				_alt = maxAlt;

			_zoom -= zoomCoeff;
			if (_zoom < minZoom)
				_zoom = minZoom;

			accepted = true;
		}

		if (accepted)
		{
			core::Proxy()->SetMapZoom(_zoom);
		}
	}

	// Throttle - Move Forward and Back
	if (data.chan3_raw > 0)
	{
		bool accepted = false;
		double dist = 0.0;
		if (data.chan3_raw == defaultChannel)
		{
			_pitch = 0;
		}
		else if (data.chan3_raw < defaultChannel)
		{
			dist -= distStep;
			_pitch = maxPitch;
			accepted = true;
		}
		else
		{
			dist += distStep;
			_pitch = -maxPitch;
			accepted = true;
		}

		if (accepted)
		{
			data::CoordinatePtr coord = data::QtCoordinate::create(_lat, _lon);
			coord = coord->AtDistanceAndAzimuth(dist, _yaw);
			_lat = coord->Lat();
			_lon = coord->Lon();
		}
	}

	// Yaw - Turn Left and Right
	if (data.chan4_raw > 0)
	{
		if (data.chan4_raw == defaultChannel)
		{
		}
		else if (data.chan4_raw < defaultChannel)
		{
			_yaw -= 2;
			if (_yaw < 0)
				_yaw = maxYaw;
		}
		else
		{
			_yaw += 2;
			if (_yaw > maxYaw)
				_yaw = 0;
		}
	}

	LOGSIM("MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE " << data.chan1_raw << " " << data.chan2_raw << " "
		   << data.chan3_raw << " " << data.chan4_raw << " ");
}

void Simulator::ProcessCommand(const mavlink_message_t& message)
{
	mavlink_command_long_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_command_long_decode(&message, &data);
	_command = data.command;

	if (_command == MAV_CMD_COMPONENT_ARM_DISARM)
		ProcessCommandArm(data);
	else 	if (_command == MAV_CMD_NAV_LOITER_UNLIM)
		ProcessCommandLoiter(data);
	else 	if (_command == MAV_CMD_NAV_RETURN_TO_LAUNCH)
		ProcessCommandRtl(data);

	LOGSIM("MAVLINK_MSG_ID_COMMAND_LONG cmd " << _command << ", params " << data.param1 << " " << data.param2
		   << " " << data.param3 << " " << data.param4);
}

void Simulator::ProcessCommandInt(const mavlink_message_t& message)
{
	mavlink_command_int_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_command_int_decode(&message, &data);
	_command = data.command;

	if (_command == MAV_CMD_COMPONENT_ARM_DISARM)
		ProcessCommandArm(data);
	else if (_command == MAV_CMD_NAV_LOITER_UNLIM)
		ProcessCommandLoiter(data);
	else if (_command == MAV_CMD_NAV_RETURN_TO_LAUNCH)
		ProcessCommandRtl(data);

	LOGSIM("MAVLINK_MSG_ID_COMMAND_INT cmd " << _command << ", params " << data.param1 << " " << data.param2
		   << " " << data.param3 << " " << data.param4);
}

void Simulator::ProcessSetMode(const mavlink_message_t& message)
{
	mavlink_set_mode_t data;
	memset(&data, 0, sizeof(data));
	mavlink_msg_set_mode_decode(&message, &data);
	_mode = data.custom_mode;

	if (_mode == FlightDevice::ModeAuto)
	{
		_loiter = false;
		_auto = true;
		StartMission();
	}

	LOGSIM("MAVLINK_MSG_ID_SET_MODE " << common::kFlightNodes[_mode]);
}

void Simulator::ProcessSetHome(const mavlink_message_t& message)
{
	LOGSIM("MAVLINK_MSG_ID_SET_HOME_POSITION");
}

void Simulator::ProcessSetPosition(const mavlink_message_t& message)
{
	LOGSIM("MAVLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT");
}

template<typename T> void Simulator::ProcessCommandArm(const T& data)
{
	if (data.param1 == 1 && data.param2 == common::kArmMagic)
	{
		_armed = true;
		LOGSIM("Device armed!");
	}
	else if (data.param1 == 0 && data.param2 == common::kDisarmMagic)
	{
		_armed = false;
		LOGSIM("Device disarmed!");
	}
}

template<typename T> void Simulator::ProcessCommandLoiter(const T& data)
{
	_loiter = true;
	_auto = false;
	_mode = FlightDevice::ModeLoiter;
}

template<typename T> void Simulator::ProcessCommandRtl(const T& data)
{
	_loiter = false;
	_auto = false;
	_mode = FlightDevice::ModeRtl;
}

void Simulator::InitMissionParams()
{
	common::AltType altType = static_cast<common::AltType>(_missionParams.Get<int>("altType"));
	_missionFrame = MAV_FRAME_GLOBAL;

	if (altType == common::AltType::Absolute)
		_missionFrame = MAV_FRAME_GLOBAL;
	else if (altType == common::AltType::Terrain)
		_missionFrame = MAV_FRAME_GLOBAL_TERRAIN_ALT;
	else if (altType == common::AltType::Relative)
		_missionFrame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
}

void Simulator::LoadMission()
{
	_missionPoints.clear();
	string fileName = common::GetCurrentDir() + PS + missionFile;
	if (!common::IsFileExists(fileName))
	{
		fileName = string("..") + PS + missionFile;
		if (!common::IsFileExists(fileName))
			return;
	}

	data::Grid grid;
	grid.LoadFile(fileName, _missionPoints, _missionParams);
	InitMissionParams();
}

void Simulator::SaveMission()
{
	string fileName = common::GetCurrentDir() + PS + missionFile;
	if (!common::IsFileExists(fileName))
		fileName = string("..") + PS + missionFile;

	data::Grid grid;
	grid.SaveFile(fileName, _missionPoints, _missionParams);
}

void Simulator::StartMission()
{
	if (!_armed)
	{
		LOGSIM("Not armed!");
		return;
	}

	_missionThread = std::thread([this]() {

		LoadMission();
		if (_missionPoints.empty())
		{
			LOGSIM("No mission points!");
			return;
		}

		for (int i = 0; i < _missionPoints.size() - 1; ++i)
		{
			if (!_auto)
				break;

			data::CoordinatePtr currentPoint = _missionPoints[i];
			data::CoordinatePtr nextPoint = _missionPoints[i + 1];
			double azimuth = currentPoint->AzimuthTo(nextPoint);
			double distance = currentPoint->DistanceTo(nextPoint);
			double step = 5;
			double currentDist = 0;
			bool work = true;
			_yaw = azimuth;

			while (work)
			{
				if (!_auto)
					break;

				if ((distance - currentDist) < step)
				{
					step = distance - currentDist;
					work = false;
				}

				data::CoordinatePtr coord = data::QtCoordinate::create(_lat, _lon);
				coord = coord->AtDistanceAndAzimuth(step, azimuth);
				_lat = coord->Lat();
				_lon = coord->Lon();
				currentDist += step;
				UpdateData();
				common::Sleep(300);
			}
		}
	});

	_missionThread.detach();
}

void Simulator::StopMission()
{
}

void Simulator::UpdateData()
{
	// GPS
	mavlink_message_t message;
	mavlink_gps_raw_int_t gps;
	memset(&gps, 0, sizeof(gps));
	memset(&message, 0, sizeof(message));
	gps.time_usec = time(NULL);
	gps.lat = _lat * 10000000;
	gps.lon = _lon * 10000000;
	gps.alt = _alt * 1000;
	gps.eph = 90;
	gps.epv = 80;
	gps.vel = UINT16_MAX; // Unknown
	gps.cog = UINT16_MAX; // Unknown
	gps.fix_type = 3; // 3D_FIX
	gps.satellites_visible = 12;

	mavlink_msg_gps_raw_int_encode(_systemId, _componentId, &message, &gps);
	Send(message);
	common::Sleep(10);

	// Attitude
	mavlink_attitude_t att;
	memset(&att, 0, sizeof(att));
	memset(&message, 0, sizeof(message));
	att.roll = _roll * _rad;
	att.pitch = _pitch * _rad;
	att.yaw = _yaw * _rad;

	mavlink_msg_attitude_encode(_systemId, _componentId, &message, &att);
	Send(message);
	common::Sleep(10);

	// vfr
	mavlink_vfr_hud_t vfr;
	memset(&vfr, 0, sizeof(vfr));
	memset(&message, 0, sizeof(message));
	vfr.alt = _alt;
	vfr.heading = _yaw;
	vfr.airspeed = 1;
	vfr.groundspeed = 0.5;

	mavlink_msg_vfr_hud_encode(_systemId, _componentId, &message, &vfr);
	Send(message);
}

void Simulator::Run()
{
	/*while (_active)
	{
		updateData();
		common::sleep(50);
	}*/
}

void Simulator::RunTest()
{
	int number = core::Broker::instance()->ActiveConnection()->Number();
	if (number == 2)
	{
		_lon = 30.316230;
		_alt = 20;
	}
	else if (number == 3)
	{
		_lon = 30.317230;
		_alt = 30;
	}
	else if (number == 4)
	{
		_lon = 30.318230;
		_alt = 40;
	}
	else if (number == 5)
	{
		_lon = 30.319230;
		_alt = 60;
	}

	while (_active)
	{
		UpdateData();

		{
			_yaw++;
			if (_yaw >= 360)
				_yaw = 0;

			_roll++;
			if (_roll >= 90)
				_roll = -90;

			_pitch++;
			if (_pitch >= 90)
				_pitch = -90;

			double lat = _lat + 0.00001;
			double lon = _lon + 0.00001;
			_lat.store(lat);
			_lon.store(lon);
		}

		common::Sleep(50);
	}
}

}
