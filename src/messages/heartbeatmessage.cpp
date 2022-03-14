#include "heartbeatmessage.h"

namespace message
{

HeartbeatMessage::HeartbeatMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void HeartbeatMessage::execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		uint32_t mode = std::any_cast<uint32_t>(_params["mode"]);
		_device->telebox()->lock();
		_device->telebox()->setMavlinkMode(mode);
		_device->telebox()->unlock();
		_state = IMessage::State::Done;
	}
}

}
