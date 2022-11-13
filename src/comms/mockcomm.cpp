#include "mockcomm.h"

#include <mavlink/common/mavlink.h>

namespace comms
{

MockComm::MockComm(const common::ConnectParams &params)
	: ICommunication(params)
	, _active(false)
{
}

MockComm::~MockComm()
{
	_active = false;
	if (_thread.joinable())
		_thread.join();
}

bool MockComm::open()
{
	return true;
}

bool MockComm::open(const std::string &host, unsigned short port)
{
	return true;
}

void MockComm::close()
{
}

void MockComm::start(const ReadFunction &callback)
{
	_callback = callback;
	_active = true;
	_thread = std::thread([this]() { run(); });
}

bool MockComm::write(const unsigned char *buffer, size_t size)
{
	return true;
}

void MockComm::run()
{
	size_t messageSize = sizeof(mavlink_message_t);
	ByteArray data(messageSize, 0);

	const int32_t lat = 59.938888 * 10000000;
	const int32_t lon = 30.315230 * 10000000;
	const int32_t alt = 21765;
	while (_active)
	{
		// Heartbeat
		mavlink_message_t message;
		mavlink_heartbeat_t heartbeat;
		memset(&heartbeat, 0, sizeof(heartbeat));
		memset(&message, 0, sizeof(message));
		heartbeat.type = MAV_TYPE_GCS;
		heartbeat.autopilot = MAV_AUTOPILOT_ARDUPILOTMEGA;
		mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);

		memcpy(&data[0], &message, messageSize);
		_callback(data);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// GPS
		mavlink_gps_raw_int_t gps;
		memset(&gps, 0, sizeof(gps));
		memset(&message, 0, sizeof(message));
		gps.time_usec = time(NULL);
		gps.lat = lat;
		gps.lon = lon;
		gps.alt = alt;
		gps.eph = UINT16_MAX; // Unknown
		gps.epv = UINT16_MAX; // Unknown
		gps.vel = UINT16_MAX; // Unknown
		gps.cog = UINT16_MAX; // Unknown
		gps.fix_type = 3; // 3D_FIX
		gps.satellites_visible = 10;
		mavlink_msg_gps_raw_int_encode(_systemId, _componentId, &message, &gps);

		memcpy(&data[0], &message, messageSize);
		_callback(data);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// Attitude
		mavlink_attitude_t att;
		memset(&att, 0, sizeof(att));
		memset(&message, 0, sizeof(message));
		att.roll = 7 * M_PI / 180;
		att.pitch = 5 * M_PI / 180;
		att.yaw = 320 * M_PI / 180;
		mavlink_msg_attitude_encode(_systemId, _componentId, &message, &att);

		memcpy(&data[0], &message, messageSize);
		_callback(data);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// vfr
		mavlink_vfr_hud_t vfr;
		memset(&vfr, 0, sizeof(vfr));
		memset(&message, 0, sizeof(message));
		vfr.alt = alt / 1000;
		vfr.heading = 320;
		vfr.airspeed = 1;
		vfr.groundspeed = 0.5;
		mavlink_msg_vfr_hud_encode(_systemId, _componentId, &message, &vfr);

		memcpy(&data[0], &message, messageSize);
		_callback(data);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

}
