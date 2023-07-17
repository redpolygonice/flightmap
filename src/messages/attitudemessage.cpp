#include "attitudemessage.h"

namespace message
{

AttitudeMessage::AttitudeMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void AttitudeMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->Telebox()->Lock();
		_device->Telebox()->roll = std::any_cast<float>(_params["roll"]);
		_device->Telebox()->pitch = std::any_cast<float>(_params["pitch"]);
		_device->Telebox()->yaw = std::any_cast<float>(_params["yaw"]);
		_device->Telebox()->Unlock();
		_state = IMessage::State::Done;
	}
}

}
