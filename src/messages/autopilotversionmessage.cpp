#include "autopilotversionmessage.h"

namespace message
{

AutopilotVersionMessage::AutopilotVersionMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void AutopilotVersionMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->Mission()->SetCapabilities(std::any_cast<uint64_t>(_params["capabilities"]));
		_state = IMessage::State::Done;
	}
}

}
