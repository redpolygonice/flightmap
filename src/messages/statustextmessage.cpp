#include "statustextmessage.h"

namespace message
{

StatusTextMessage::StatusTextMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void StatusTextMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->Telebox()->Lock();
		_device->Telebox()->statustext = std::any_cast<string>(_params["statustext"]);
		_device->Telebox()->Unlock();
		_state = IMessage::State::Done;
	}
}

}
