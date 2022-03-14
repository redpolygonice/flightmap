#include "statustextmessage.h"

namespace message
{

StatusTextMessage::StatusTextMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void StatusTextMessage::execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->telebox()->lock();
		_device->telebox()->statustext = std::any_cast<string>(_params["statustext"]);
		_device->telebox()->unlock();
		_state = IMessage::State::Done;
	}
}

}
