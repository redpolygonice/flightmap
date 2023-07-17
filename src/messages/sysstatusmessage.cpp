#include "sysstatusmessage.h"

namespace message
{

SysStatusMessage::SysStatusMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void SysStatusMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;
		_device->Telebox()->Lock();
		_device->Telebox()->voltage_battery = std::any_cast<unsigned short>(_params["voltage_battery"]);
		_device->Telebox()->current_battery = std::any_cast<short>(_params["current_battery"]);
		_device->Telebox()->drop_rate_comm = std::any_cast<unsigned short>(_params["drop_rate_comm"]);
		_device->Telebox()->errors_comm = std::any_cast<unsigned short>(_params["errors_comm"]);
		_device->Telebox()->battery_remaining = std::any_cast<char>(_params["battery_remaining"]);
		_device->Telebox()->Unlock();
		_state = IMessage::State::Done;
	}
}

}
