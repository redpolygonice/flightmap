#include "gpsmessage.h"

namespace message
{

GpsMessage::GpsMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void GpsMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->Telebox()->Lock();
		_device->Telebox()->lat = std::any_cast<float>(_params["lat"]);
		_device->Telebox()->lon = std::any_cast<float>(_params["lon"]);
		_device->Telebox()->altgps = std::any_cast<float>(_params["altgps"]);
		_device->Telebox()->eph = std::any_cast<float>(_params["eph"]);
		_device->Telebox()->epv = std::any_cast<float>(_params["epv"]);
		_device->Telebox()->fix_type = std::any_cast<uint8_t>(_params["fix_type"]);
		_device->Telebox()->satellites_visible = std::any_cast<uint8_t>(_params["satellites_visible"]);
		_device->Telebox()->gpstype = _device->Telebox()->GpsTypeToString(static_cast<GPS_FIX_TYPE>(_device->Telebox()->fix_type));
		_device->Telebox()->Unlock();
		_state = IMessage::State::Done;
	}
}

}
