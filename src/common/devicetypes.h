#ifndef DEVICEPROPS_H
#define DEVICEPROPS_H

#include "types.h"

namespace common
{

constexpr float kArmMagic = 2989.0f;
constexpr float kDisarmMagic = 21196.0f;

// UART device name
#ifdef WIN32
#define UART_NAME "COM" // COM5 - radio module
#define PIXHAWK_NAME "COM" // COM5 - pixhawk device
#else
#define UART_NAME "ttyUSB" // ttyUSB0 - radio module
#define PIXHAWK_NAME "ttyACM" // ttyACM0 - pixhawk device
#endif

enum class ProtocolType
{
	Tcp,
	Udps,
	Udpc,
	Uart
};

enum class DeviceType
{
	Pixhawk,
	Raspi,
	Mock
};

enum class AltType
{
	Absolute,
	Relative,
	Terrain
};

struct ConnectParams
{
	string protocol = "TCP";
	string device = "Pixhawk";
	string host = "192.168.10.1";
	uint16_t port = 15000;
	uint32_t baudrate = 57600;
};

enum OnboardDeviceType
{
	BeepDevice = 0,
	AntennaDevice,
	AutolandDevice,
	MicrophoneDevice,
	SpeakerDevice,
	CameraDevice
};

enum class Severity
{
	Emergency = 0,
	Alert,
	Critical,
	Error,
	Warning,
	Notice,
	Informational,
	Debug
};

static StringList kFlightNodes = {
	"Stabilize",
	"Acrobatic",
	"Alt Hold ",
	"Auto",
	"Guided",
	"Loiter",
	"RTL",
	"Circle",
	"Position",
	"Land",
	"OF_Loiter",
	"Drift",
	"None",
	"Sport",
	"Flip",
	"Auto Tune",
	"Pos Hold"
};

}

#endif // DEVICEPROPS_H
