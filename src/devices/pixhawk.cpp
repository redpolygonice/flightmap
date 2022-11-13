#include "pixhawk.h"
#include "common/log.h"
#include "common/common.h"
#include "messages/dispatcher.h"
#include "data/mission.h"
#include "data/missionitem.h"

#include <mavlink/common/mavlink.h>
#include <mavlink/ardupilotmega/ardupilotmega.h>

namespace device
{

Pixhawk::Pixhawk(const comms::CommunicationPtr &comm)
	: IDevice(comm)
	, _active(false)
{
	_type = common::DeviceType::Pixhawk;
}

device::Pixhawk::~Pixhawk()
{
}

std::string Pixhawk::name() const
{
	return _comm->connectParams().device + " " + _comm->connectParams().protocol;
}

bool Pixhawk::start()
{
	if (_active)
		return true;

	_active = true;
	_messageFactory.init(shared_from_this());
	_telebox = data::TeleBox::create();
	startHeartbeat();

	// Request telemetry data
	for (int i = 0; i < 5; ++i)
	{
		if (requestData())
			break;
	}

	// Start reading from pixhawk
	_comm->start([this](const ByteArray &data) { readData(data); });
	_processThread = std::thread([this]() { processData(); });
	return true;
}

void Pixhawk::stop()
{
	_active = false;

	if (_heartbeatThread.joinable())
		_heartbeatThread.join();

	if (_readThread.joinable())
		_readThread.join();

	if (_processThread.joinable())
		_processThread.join();

	_comm->close();
}

bool Pixhawk::createMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params)
{
	if (_mission == nullptr)
		_mission = data::Mission::create(shared_from_this());

	return _mission->createPoints(points, mission, params);
}

void Pixhawk::clearMission()
{
	if (_mission == nullptr)
		return;

	_mission->clear();
}

void Pixhawk::writeMission(const data::CoordinateList &points, const common::AnyMap &params)
{
	if (_mission == nullptr)
		return;

	_mission->write(points, params);
}

void Pixhawk::readMission(data::CoordinateList &points)
{
	if (_mission == nullptr)
		_mission = data::Mission::create(shared_from_this());

	_mission->read(points);
}

void Pixhawk::sendCommand(uint16_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
	mavlink_message_t data;
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

	if (!sendMessage(data))
		LOGE("Message mavlink_command_long_t error!");
}

void Pixhawk::sendCommandInt(uint16_t cmd, float param1, float param2, float param3, float param4, float x, float y, float z)
{
	mavlink_message_t data;
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

	if (!sendMessage(data))
		LOGE("Message mavlink_command_int_t error!");
}

void Pixhawk::RcChannelsOverride(uint16_t ch1, uint16_t ch2, uint16_t ch3, uint16_t ch4, uint16_t ch5, uint16_t ch6)
{
	mavlink_message_t data;
	mavlink_rc_channels_override_t message;
	memset(&message, 0, sizeof(message));

	message.target_component = MAV_COMP_ID_ALL;
	message.target_system = _targetSysid;
	message.chan1_raw = ch1;
	message.chan2_raw = ch2;
	message.chan3_raw = ch3;
	message.chan4_raw = ch4;
	message.chan5_raw = ch5;
	message.chan6_raw = ch6;
	message.chan7_raw = UINT16_MAX;
	message.chan8_raw = UINT16_MAX;
	mavlink_msg_rc_channels_override_encode(_systemId, _componentId, &data, &message);

	if (!sendMessage(data))
		LOGE("Message mavlink_mission_item_t error!");
}

void Pixhawk::setMode(uint32_t mode)
{
	mavlink_message_t data;
	mavlink_set_mode_t message;
	memset(&message, 0, sizeof(message));

	message.target_system = MAV_AUTOPILOT_RESERVED;
	message.base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
	message.custom_mode = mode;
	mavlink_msg_set_mode_encode(_systemId, _componentId, &data, &message);

	if (!sendMessage(data))
		LOGE("Message mavlink_set_mode_t error!");
}

void Pixhawk::setHome(float lat, float lon, float alt)
{
	mavlink_message_t data;
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

	if (!sendMessage(data))
		LOGE("Message mavlink_set_home_position_t error!");
}

void Pixhawk::setPosition(double lat, double lon, double alt)
{
	// for mavlink SET_POSITION_TARGET messages
	const ushort MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE = ((1 << 0) | (1 << 1) | (1 << 2));
	const ushort MAVLINK_SET_POS_TYPE_MASK_ALT_IGNORE = ((0 << 0) | (0 << 1) | (1 << 2));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE = ((1 << 3) | (1 << 4) | (1 << 5));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE = ((1 << 6) | (1 << 7) | (1 << 8));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_FORCE = ((1 << 9));
	//const ushort MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE = ((1 << 10) | (1 << 11));

	mavlink_message_t data;
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

	if (!sendMessage(data))
		LOGE("Message mavlink_set_position_target_global_int_t error!");
}

bool Pixhawk::sendMessage(mavlink_message_t &message)
{
	if (_comm->write(reinterpret_cast<const unsigned char *>(&message), sizeof(message)) > 0)
		return true;
	return false;
}

void Pixhawk::startHeartbeat()
{
	_heartbeatThread = std::thread([this]()
	{
		mavlink_message_t message;
		mavlink_heartbeat_t heartbeat;
		memset(&heartbeat, 0, sizeof(heartbeat));
		heartbeat.type = MAV_TYPE_GENERIC;
		heartbeat.autopilot = MAV_AUTOPILOT_GENERIC;
		mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);

		while (_active)
		{
			if (!_comm->write((const unsigned char *)&message, sizeof(message)))
			{
				LOGE("[Pixhawk] Can't send Heartbeat");
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	});
}

bool Pixhawk::requestData()
{
	mavlink_message_t message;
	memset(&message, 0, sizeof(message));
	mavlink_request_data_stream_t data;
	memset(&data, 0, sizeof(data));
	data.req_message_rate = 2;
	data.req_stream_id = MAV_DATA_STREAM_ALL;
	data.start_stop = 1;
	data.target_system = _targetSysid;
	data.target_component = MAV_COMP_ID_ALL;

	bool result = false;
	mavlink_msg_request_data_stream_encode(_systemId, _componentId, &message, &data);
	if (_comm->write((const unsigned char *)&message, sizeof(message)))
	{
		result = true;
		LOG("[Pixhawk] requestData OK");
	}
	else
	{
		result = false;
		LOGE("[Pixhawk] requestData FAILED");
	}

	return result;
}

void Pixhawk::readData(const ByteArray &data)
{
	std::lock_guard<std::mutex> lock(_mutexData);
	_data.insert(_data.end(), data.begin(), data.end());
}

bool Pixhawk::processData()
{
	mavlink_message_t message;
	mavlink_status_t status;

	while (_active)
	{
		_mutexData.lock();
		bool empty = _data.empty();
		_mutexData.unlock();

		if (empty)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		_mutexData.lock();
		ByteArray temp(std::move(_data));
		_mutexData.unlock();

		for (size_t i = 0; i < temp.size(); ++i)
		{
			if (mavlink_parse_char(0, temp[i], &message, &status))
			{
				_sequence = message.seq;
				_targetSysid = message.sysid;
				_targetCompid = message.compid;

				if (_mission != nullptr)
				{
					if (_mission->processMessage(message))
						continue;
				}

				message::MessagePtr messagePtr = _messageFactory.create(message);
				if (messagePtr != nullptr)
					message::Dispatcher::instance()->add(messagePtr);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return true;
}

}
