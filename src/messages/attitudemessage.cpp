#include "attitudemessage.h"

namespace message
{

AttitudeMessage::AttitudeMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void AttitudeMessage::execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->telebox()->lock();
		_device->telebox()->roll = std::any_cast<float>(_params["roll"]);
		_device->telebox()->pitch = std::any_cast<float>(_params["pitch"]);
		_device->telebox()->yaw = std::any_cast<float>(_params["yaw"]);
		_device->telebox()->unlock();
		_state = IMessage::State::Done;
	}
}

}
