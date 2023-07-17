#include "telebox.h"
#include "common/devicetypes.h"

namespace data
{

TeleBox::TeleBox()
{
}

void TeleBox::SetMavlinkAttitude(const mavlink_attitude_t &msg)
{
	roll = static_cast<float>(msg.roll * 180 / M_PI);
	pitch = static_cast<float>(msg.pitch * 180 / M_PI);
	yaw = static_cast<float>(msg.yaw * 180 / M_PI);
}

void TeleBox::SetMavlinkVfrHud(const mavlink_vfr_hud_t &msg)
{
	airspeed = msg.airspeed;
	groundspeed = msg.groundspeed;
	altmsl = msg.alt;
	climb = msg.climb;
	heading = msg.heading;
	alt = altmsl - homealt;
}

void TeleBox::SetMavlinkGpsRawInt(const mavlink_gps_raw_int_t &msg)
{
	lat = static_cast<float>(msg.lat / 10000000.0f);
	lon = static_cast<float>(msg.lon / 10000000.0f);
	altgps = static_cast<float>(msg.alt / 1000.0f);
	eph = msg.eph / 100;
	epv = msg.epv / 100;
	fix_type = msg.fix_type;
	satellites_visible = msg.satellites_visible;
	gpstype = GpsTypeToString(static_cast<GPS_FIX_TYPE>(fix_type));
}

void TeleBox::SetMavlinkSysStatus(const mavlink_sys_status_t &msg)
{
	voltage_battery = msg.voltage_battery;
	current_battery = msg.current_battery;
	drop_rate_comm = msg.drop_rate_comm;
	errors_comm = msg.errors_comm;
	battery_remaining = msg.battery_remaining;
}

void TeleBox::SetMavlinkStatusText(const mavlink_statustext_t &msg)
{
	statustext = msg.text;
}

void TeleBox::SetMavlinkMode(uint32_t mode)
{
	flightmode = common::kFlightNodes[mode];
}

string TeleBox::GpsTypeToString(GPS_FIX_TYPE type)
{
	string text = "";
	switch (type)
	{
	case GPS_FIX_TYPE_NO_GPS:
		text = "NO_GPS";
		break;
	case GPS_FIX_TYPE_NO_FIX:
		text = "NO_FIX";
		break;
	case GPS_FIX_TYPE_2D_FIX:
		text = "2D_FIX";
		break;
	case GPS_FIX_TYPE_3D_FIX:
		text = "3D_FIX";
		break;
	case GPS_FIX_TYPE_DGPS:
		text = "DGPS";
		break;
	case GPS_FIX_TYPE_RTK_FLOAT:
		text = "RTK_FLOAT";
		break;
	case GPS_FIX_TYPE_RTK_FIXED:
		text = "RTK_FIXED";
		break;
	case GPS_FIX_TYPE_STATIC:
		text = "STATIC";
		break;
	case GPS_FIX_TYPE_PPP:
		text = "PPP";
		break;
	default:
		break;
	}

	return text;
}

}
