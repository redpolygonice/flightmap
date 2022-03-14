#include "vfrmessage.h"

namespace message
{

VfrMessage::VfrMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void VfrMessage::execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->telebox()->lock();
		_device->telebox()->airspeed = std::any_cast<float>(_params["airspeed"]);
		_device->telebox()->groundspeed = std::any_cast<float>(_params["groundspeed"]);
		_device->telebox()->altmsl = std::any_cast<float>(_params["altmsl"]);
		_device->telebox()->climb = std::any_cast<float>(_params["climb"]);
		_device->telebox()->heading = std::any_cast<short>(_params["heading"]);
		_device->telebox()->alt = std::any_cast<float>(_params["altmsl"]) - _device->telebox()->homealt;
		_device->telebox()->unlock();
		_state = IMessage::State::Done;
	}
}

}
