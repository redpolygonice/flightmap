#include "gpsmessage.h"

namespace message
{

GpsMessage::GpsMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void GpsMessage::execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->telebox()->lock();
		_device->telebox()->lat = std::any_cast<float>(_params["lat"]);
		_device->telebox()->lon = std::any_cast<float>(_params["lon"]);
		_device->telebox()->altgps = std::any_cast<float>(_params["altgps"]);
		_device->telebox()->eph = std::any_cast<float>(_params["eph"]);
		_device->telebox()->epv = std::any_cast<float>(_params["epv"]);
		_device->telebox()->fix_type = std::any_cast<uint8_t>(_params["fix_type"]);
		_device->telebox()->satellites_visible = std::any_cast<uint8_t>(_params["satellites_visible"]);
		_device->telebox()->gpstype = _device->telebox()->gpsTypeToString(static_cast<GPS_FIX_TYPE>(_device->telebox()->fix_type));
		_device->telebox()->unlock();
		_state = IMessage::State::Done;
	}
}

}
