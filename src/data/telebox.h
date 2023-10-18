#ifndef TELEBOX_H
#define TELEBOX_H

#include "common/types.h"
#include <atomic>

namespace data
{

class TeleBox;
typedef std::shared_ptr<TeleBox> TeleBoxPtr;

// Telemetry box
class TeleBox
{
private:
	std::shared_mutex _mutex;
	std::atomic_bool _active = false;

public:
	// Current flight mode
	uint32_t mode = 0;
	time_t _hbTime = 0;

	// Current mission number
	unsigned short mission_seq = 0;

	// Relative alt
	float alt = 0.0f;

	// Home position
	float homelat = 0.0f;
	float homelon = 0.0f;
	float homealt = 0.0f;

	// mavlink_attitude_t
	float roll = 0.0; /*< [rad] Roll angle (-pi..+pi)*/
	float pitch = 0.0; /*< [rad] Pitch angle (-pi..+pi)*/
	float yaw = 0.0; /*< [rad] Yaw angle (-pi..+pi)*/

	// mavlink_vfr_hud_t
	float airspeed = 0.0; /*< [m/s] Current indicated airspeed (IAS).*/
	float groundspeed = 0.0; /*< [m/s] Current ground speed.*/
	float altmsl = 0.0; /*< [m] Current altitude (MSL).*/
	float climb = 0.0; /*< [m/s] Current climb rate.*/
	int16_t heading = 0; /*< [deg] Current heading in compass units (0-360, 0=north).*/

	// mavlink_gps_raw_int_t
	float lat = 0.0; /*< [degE7] Latitude (WGS84, EGM96 ellipsoid)*/
	float lon = 0.0; /*< [degE7] Longitude (WGS84, EGM96 ellipsoid)*/
	float altgps = 0.0; /*< [mm] Altitude (MSL). Positive for up. Note that virtually all GPS modules provide the MSL altitude in addition to the WGS84 altitude.*/
	float eph = 0.0; /*<  GPS HDOP horizontal dilution of position (unitless). If unknown, set to: UINT16_MAX*/
	float epv = 0.0; /*<  GPS VDOP vertical dilution of position (unitless). If unknown, set to: UINT16_MAX*/
	uint8_t fix_type = 0; /*<  GPS fix type.*/
	uint8_t satellites_visible = 0; /*<  Number of satellites visible. If unknown, set to 255*/

	// mavlink_sys_status_t
	uint16_t voltage_battery = 0; /*< [mV] Battery voltage, UINT16_MAX: Voltage not sent by autopilot*/
	int16_t current_battery = 0; /*< [cA] Battery current, -1: Current not sent by autopilot*/
	uint16_t drop_rate_comm = 0; /*< [c%] Communication drop rate, (UART, I2C, SPI, CAN), dropped packets on all links (packets that were corrupted on reception on the MAV)*/
	uint16_t errors_comm = 0; /*<  Communication errors (UART, I2C, SPI, CAN), dropped packets on all links (packets that were corrupted on reception on the MAV)*/
	int16_t battery_remaining = 0; /*< [%] Battery energy remaining, -1: Battery remaining energy not sent by autopilot*/

	// mavlink_statustext_t
	string statustext;

	// GPS type
	string gpstype = "NO_GPS";

	// flight mode
	string flightmode;

public:
	TeleBox();

public:
	static TeleBoxPtr Create() { return std::make_shared<TeleBox>(); }
	void Lock() { _mutex.lock_shared(); }
	void Unlock() { _mutex.unlock_shared(); }
	void Enable() { _active = true; }
	void Disable() { _active = false; }
	bool IsActive() const { return _active; }
	void SetMavlinkMode(uint32_t mode);
	void SetHbTime(time_t arg) { _hbTime = arg; }
	void SetMavlinkAttitude(const mavlink_attitude_t &msg);
	void SetMavlinkVfrHud(const mavlink_vfr_hud_t &msg);
	void SetMavlinkGpsRawInt(const mavlink_gps_raw_int_t &msg);
	void SetMavlinkSysStatus(const mavlink_sys_status_t &msg);
	void SetMavlinkStatusText(const mavlink_statustext_t &msg);
	string GpsTypeToString(GPS_FIX_TYPE type);
};

}

#endif // TELEBOX_H
