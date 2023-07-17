#include "heartbeatmessage.h"

namespace message
{

HeartbeatMessage::HeartbeatMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void HeartbeatMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		uint32_t mode = std::any_cast<uint32_t>(_params["mode"]);
		_device->Telebox()->Lock();
		_device->Telebox()->SetMavlinkMode(mode);
		_device->Telebox()->SetHbTime(std::time(nullptr));
		_device->Telebox()->Unlock();
		_state = IMessage::State::Done;
	}
}

}
