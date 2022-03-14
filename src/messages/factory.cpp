#include "factory.h"

#include "heartbeatmessage.h"
#include "attitudemessage.h"
#include "vfrmessage.h"
#include "gpsmessage.h"
#include "sysstatusmessage.h"
#include "statustextmessage.h"

namespace message
{

Factory::Factory()
	: _device(nullptr)
{
}

void Factory::init(const device::DevicePtr &device)
{
	_device = device;
}

void Factory::initDefaultParams(const MessagePtr &message, const mavlink_message_t &mavlink, const std::string &name)
{
	message->_id = mavlink.msgid;
	message->_name = name;
	message->_sequence = mavlink.seq;
	message->_state = IMessage::State::Idle;
	message->_direction = IMessage::Direction::FromDevice;
}

MessagePtr Factory::create(const mavlink_message_t &mavlink)
{
	if (_device == nullptr)
		return nullptr;

	MessagePtr message = nullptr;

	if (mavlink.msgid == MAVLINK_MSG_ID_HEARTBEAT)
	{
		mavlink_heartbeat_t heartbeat;
		mavlink_msg_heartbeat_decode(&mavlink, &heartbeat);

		message = HeartbeatMessage::create(_device);
		initDefaultParams(message, mavlink, "heartbeat");
		message->_params["mode"] = heartbeat.custom_mode;
	}
	else if (mavlink.msgid == MAVLINK_MSG_ID_ATTITUDE)
	{
		mavlink_attitude_t attitude;
		mavlink_msg_attitude_decode(&mavlink, &attitude);

		message = AttitudeMessage::create(_device);
		initDefaultParams(message, mavlink, "attitude");
		message->_params["roll"] = static_cast<float>(attitude.roll * 180 / M_PI);
		message->_params["pitch"] = static_cast<float>(attitude.pitch * 180 / M_PI);
		message->_params["yaw"] = static_cast<float>(attitude.yaw * 180 / M_PI);
	}
	else if (mavlink.msgid == MAVLINK_MSG_ID_VFR_HUD)
	{
		mavlink_vfr_hud_t vfr;
		mavlink_msg_vfr_hud_decode(&mavlink, &vfr);

		message = VfrMessage::create(_device);
		initDefaultParams(message, mavlink, "vfr");
		message->_params["airspeed"] = vfr.airspeed;
		message->_params["groundspeed"] = vfr.groundspeed;
		message->_params["altmsl"] = vfr.alt;
		message->_params["climb"] = vfr.climb;
		message->_params["heading"] = vfr.heading;
	}
	else if (mavlink.msgid == MAVLINK_MSG_ID_GPS_RAW_INT)
	{
		mavlink_gps_raw_int_t gps;
		mavlink_msg_gps_raw_int_decode(&mavlink, &gps);

		message = GpsMessage::create(_device);
		initDefaultParams(message, mavlink, "gps");
		message->_params["lat"] = static_cast<float>(gps.lat / 10000000.0f);
		message->_params["lon"] = static_cast<float>(gps.lon / 10000000.0f);
		message->_params["altgps"] = static_cast<float>(gps.alt / 1000.0f);
		message->_params["eph"] = static_cast<float>(gps.eph / 100.0f);
		message->_params["epv"] = static_cast<float>(gps.epv / 100.0f);
		message->_params["fix_type"] = gps.fix_type;
		message->_params["satellites_visible"] = gps.satellites_visible;
	}
	else if (mavlink.msgid == MAVLINK_MSG_ID_SYS_STATUS)
	{
		mavlink_sys_status_t status;
		mavlink_msg_sys_status_decode(&mavlink, &status);

		message = SysStatusMessage::create(_device);
		initDefaultParams(message, mavlink, "sysstatus");
		message->_params["voltage_battery"] = status.voltage_battery;
		message->_params["current_battery"] = status.current_battery;
		message->_params["drop_rate_comm"] = status.drop_rate_comm;
		message->_params["errors_comm"] = status.errors_comm;
		message->_params["battery_remaining"] = status.battery_remaining;
	}
	else if (mavlink.msgid == MAVLINK_MSG_ID_STATUSTEXT)
	{
		mavlink_statustext_t status;
		mavlink_msg_statustext_decode(&mavlink, &status);

		message = StatusTextMessage::create(_device);
		initDefaultParams(message, mavlink, "statustext");
		message->_params["statustext"] = string(status.text);
	}

	return message;
}

}
