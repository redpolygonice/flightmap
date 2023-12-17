#include "mock.h"
#include "common/log.h"
#include "common/common.h"
#include "messages/dispatcher.h"

namespace device
{

Mock::Mock(const comms::CommunicationPtr &comm)
	: FlightDevice(comm)
	, _active(false)
{
	_type = common::DeviceType::Mock;
}

std::string Mock::Name() const
{
	return _comm->connectParams().device;
}

Mock::~Mock()
{
}

bool Mock::Start()
{
	if (_active)
		return true;

	_active = true;
	_telebox = data::TeleBox::Create();
	_telebox->Enable();
	_messageFactory.Init(shared_from_this());
	_comm->Start([this](const ByteArray &data) { ReadData(data); });
	_processThread = std::thread([this]() { ProcessData(); });
	return true;
}

void Mock::Stop()
{
	_active = false;
	_telebox->Disable();

	if (_processThread.joinable())
		_processThread.join();

	_comm->Close();
}

void Mock::ProcessData()
{
	mavlink_message_t message;
	mavlink_status_t status;

	while (_active)
	{
		if (_data.Empty())
		{
			common::Sleep(20);
			continue;
		}

		ByteArrayPtr temp;
		_data.Pop(std::move(temp));

		for (size_t i = 0; i < temp->size(); ++i)
		{
			if (mavlink_parse_char(0, temp->at(i), &message, &status))
			{
				_sequence = message.seq;
				_targetSysid = message.sysid;
				_targetCompid = message.compid;

				if (_mission != nullptr)
				{
					if (std::find(missionTypes.begin(), missionTypes.end(), message.msgid) != missionTypes.end())
					{
						_mission->ProcessMessage(message);
						continue;
					}
				}

				message::MessagePtr messagePtr = _messageFactory.Create(message);
				if (messagePtr != nullptr)
					message::Dispatcher::instance()->Add(messagePtr);
			}
		}

		common::Sleep(50);
	}
}

void Mock::ReadData(const ByteArray &data)
{
	ByteArray temp(0, data.size());
	std::copy(data.begin(), data.end(), std::back_inserter(temp));
	_data.Push(std::move(std::make_unique<ByteArray>(temp)));
}

}
