#include "pixhawk.h"
#include "common/log.h"
#include "common/common.h"
#include "messages/dispatcher.h"
#include <cstdint>

namespace device
{

Pixhawk::Pixhawk(const comms::CommunicationPtr &comm)
	: FlightDevice(comm)
	, _active(false)
{
	_type = common::DeviceType::Pixhawk;
}

device::Pixhawk::~Pixhawk()
{
}

std::string Pixhawk::Name() const
{
	return _comm->connectParams().device + " " + _comm->connectParams().protocol;
}

bool Pixhawk::Start()
{
	if (_active)
		return true;

	// Initialize
	_active = true;
	_messageFactory.Init(shared_from_this());
	_telebox = data::TeleBox::Create();
	_telebox->Enable();

	// Start reading from pixhawk
	_comm->Start([this](const ByteArray &data) { ReadData(data); });
	_processThread = std::thread([this]() { ProcessData(); });

	// Request data
	RequestData();
	StartHeartbeat();
	return true;
}

void Pixhawk::Stop()
{
	_active = false;
	_telebox->Disable();

	if (_heartbeatThread.joinable())
		_heartbeatThread.join();

	if (_processThread.joinable())
		_processThread.join();

	_comm->Close();
}

bool Pixhawk::WaitHeartbeat()
{
	int heartbeatTimeout = 30;
	while (heartbeatTimeout-- >= 0)
	{
		if (_telebox->_hbTime > 0)
			return true;

		common::Sleep(1000);
	}

	return false;
}

void Pixhawk::StartHeartbeat()
{
	_heartbeatThread = std::thread([this]()
	{
		mavlink_message_t message;
		memset(&message, 0, sizeof(message));
		mavlink_heartbeat_t heartbeat;
		memset(&heartbeat, 0, sizeof(heartbeat));

		heartbeat.type = MAV_TYPE_GENERIC;
		heartbeat.autopilot = MAV_AUTOPILOT_GENERIC;
		heartbeat.system_status = 1;
		heartbeat.custom_mode = 0;
		heartbeat.base_mode = 0;
		mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);

		while (_active)
		{
			if (!Send(message))
				LOGE("[Pixhawk] Can't send Heartbeat!");
			common::Sleep(1000);
		}
	});
}

bool Pixhawk::RequestData()
{
	mavlink_message_t message;
	memset(&message, 0, sizeof(message));
	mavlink_request_data_stream_t data;
	memset(&data, 0, sizeof(data));

	data.req_message_rate = 2;
	data.req_stream_id = MAV_DATA_STREAM_ALL;
	data.start_stop = 1;
	data.target_system = _targetSysid;
	data.target_component = MAV_COMP_ID_ALL;
	mavlink_msg_request_data_stream_encode(_systemId, _componentId, &message, &data);

	if (!Send(message))
	{
		LOGE("[Pixhawk] Can't request data!");
		return false;
	}

	return true;
}

void Pixhawk::ReadData(const ByteArray &data)
{
	ByteArray temp(0, data.size());
	std::copy(data.begin(), data.end(), std::back_inserter(temp));
	_data.Push(std::move(std::make_unique<ByteArray>(temp)));
}

bool Pixhawk::ProcessData()
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

	return true;
}

}
