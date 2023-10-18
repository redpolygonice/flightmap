#include "commandmessage.h"

namespace message
{

CommandMessage::CommandMessage(const device::DevicePtr &device)
	: IMessage(device)
{
}

void CommandMessage::Execute()
{
	if (_direction == IMessage::Direction::FromDevice)
	{
		_state = IMessage::State::Progress;

		if (std::any_cast<uint16_t>(_params["cmd"]) == MAV_CMD_VIDEO_START_STREAMING)
			StartStreaming();
		else if (std::any_cast<uint16_t>(_params["cmd"]) == MAV_CMD_VIDEO_STOP_STREAMING)
			StopStreaming();

		_state = IMessage::State::Done;
	}
}

void CommandMessage::StartStreaming()
{
}

void CommandMessage::StopStreaming()
{
}

}
