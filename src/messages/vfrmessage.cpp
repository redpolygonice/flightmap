#include "vfrmessage.h"

namespace message
{

VfrMessage::VfrMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void VfrMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->Telebox()->Lock();
		_device->Telebox()->airspeed = std::any_cast<float>(_params["airspeed"]);
		_device->Telebox()->groundspeed = std::any_cast<float>(_params["groundspeed"]);
		_device->Telebox()->altmsl = std::any_cast<float>(_params["altmsl"]);
		_device->Telebox()->climb = std::any_cast<float>(_params["climb"]);
		_device->Telebox()->heading = std::any_cast<short>(_params["heading"]);
		_device->Telebox()->alt = std::any_cast<float>(_params["altmsl"]) - _device->Telebox()->homealt;
		_device->Telebox()->Unlock();
		_state = IMessage::State::Done;
	}
}

}
