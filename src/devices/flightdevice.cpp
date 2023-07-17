#include "flightdevice.h"
#include "data/mission.h"

namespace device
{

bool FlightDevice::Send(const mavlink_message_t &message)
{
	if (_comm->Write(reinterpret_cast<const unsigned char *>(&message), sizeof(message)) > 0)
		return true;
	return false;
}

bool FlightDevice::CreateMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params)
{
	if (_mission == nullptr)
		_mission = data::Mission::Create(shared_from_this());

	return _mission->CreatePoints(points, mission, params);
}

void FlightDevice::ClearMission()
{
	if (_mission == nullptr)
		return;

	_mission->Clear();
}

void FlightDevice::WriteMission(const data::CoordinateList &points, const common::AnyMap &params)
{
	if (_mission == nullptr)
		_mission = data::Mission::Create(shared_from_this());

	_mission->Write(points, params);
}

void FlightDevice::ReadMission(data::CoordinateList &points)
{
	if (_mission == nullptr)
		_mission = data::Mission::Create(shared_from_this());

	_mission->Read(points);
}

void FlightDevice::SendCommand(uint16_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_command_long_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.command = cmd;
	message.param1 = param1;
	message.param2 = param2;
	message.param3 = param3;
	message.param4 = param4;
	message.param5 = param5;
	message.param6 = param6;
	message.param7 = param7;
	mavlink_msg_command_long_encode(_systemId, _componentId, &data, &message);

	if (!Send(data))
		LOGE("Message mavlink_command_long_t error!");
}

void FlightDevice::SendCommandInt(uint16_t cmd, float param1, float param2, float param3, float param4, float x, float y, float z)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_command_int_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.command = cmd;
	message.frame = MAV_FRAME_GLOBAL;
	message.param1 = param1;
	message.param2 = param2;
	message.param3 = param3;
	message.param4 = param4;
	message.x = (int)(x * 1.0e7);
	message.y = (int)(y * 1.0e7);
	message.z = z;
	mavlink_msg_command_int_encode(_systemId, _componentId, &data, &message);

	if (!Send(data))
		LOGE("Message mavlink_command_int_t error!");
}

void FlightDevice::RcChannelsOverride(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_rc_channels_override_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.chan1_raw = ch1;
	message.chan2_raw = ch2;
	message.chan3_raw = ch3;
	message.chan4_raw = ch4;
	mavlink_msg_rc_channels_override_encode(_systemId, _componentId, &data, &message);

	if (!Send(data))
		LOGE("Message mavlink_mission_item_t error!");
}

void FlightDevice::SetMode(uint32_t mode)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_set_mode_t message;
	memset(&message, 0, sizeof(message));

	message.target_system = MAV_AUTOPILOT_RESERVED;
	message.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
	message.custom_mode = mode;
	mavlink_msg_set_mode_encode(_systemId, _componentId, &data, &message);

	if (!Send(data))
		LOGE("Message mavlink_set_mode_t error!");
}

void FlightDevice::SetHome(float lat, float lon, float alt)
{
	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_set_home_position_t message;
	memset(&message, 0, sizeof(message));

	message.target_system = _targetSysid;
	message.latitude = 0;
	message.longitude = 0;
	message.altitude = 0;
	message.x = lat;
	message.y = lon;
	message.z = alt;
	mavlink_msg_set_home_position_encode(_systemId, _componentId, &data, &message);

	if (!Send(data))
		LOGE("Message mavlink_set_home_position_t error!");
}

void FlightDevice::SetPosition(double lat, double lon, double alt)
{
	// for mavlink SET_POSITION_TARGET messages
	const uint16_t MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE = ((1 << 0) | (1 << 1) | (1 << 2));
	const uint16_t MAVLINK_SET_POS_TYPE_MASK_ALT_IGNORE = ((0 << 0) | (0 << 1) | (1 << 2));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE = ((1 << 3) | (1 << 4) | (1 << 5));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE = ((1 << 6) | (1 << 7) | (1 << 8));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_FORCE = ((1 << 9));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE = ((1 << 10) | (1 << 11));

	mavlink_message_t data;
	memset(&data, 0, sizeof(data));
	mavlink_set_position_target_global_int_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.coordinate_frame = MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;
	message.lat_int = (int)(lat * 1e7);
	message.lon_int = (int)(lon * 1e7);
	message.alt = (float)alt;
	message.vx = 0;
	message.vy = 0;
	message.vz = 0;
	message.afx = 0;
	message.afy = 0;
	message.afz = 0;
	message.yaw = 0;
	message.yaw_rate = 0;
	message.type_mask = UINT16_MAX;

	if (lat != 0 && lon != 0)
		message.type_mask -= MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE;
	if (lat == 0 && lon == 0)
		message.type_mask -= MAVLINK_SET_POS_TYPE_MASK_ALT_IGNORE;

	mavlink_msg_set_position_target_global_int_encode(_systemId, _componentId, &data, &message);

	if (!Send(data))
		LOGE("Message mavlink_set_position_target_global_int_t error!");
}


}
