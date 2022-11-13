#include "autopilotversionmessage.h"

namespace message
{

AutopilotVersionMessage::AutopilotVersionMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void AutopilotVersionMessage::execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->mission()->setCapabilities(std::any_cast<uint64_t>(_params["capabilities"]));
		_state = IMessage::State::Done;
	}
}

}
