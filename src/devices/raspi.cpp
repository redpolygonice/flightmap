#include "raspi.h"
#include "common/log.h"
#include "common/common.h"
#include "messages/dispatcher.h"
#include <cstdint>

namespace device
{

Raspi::Raspi(const comms::CommunicationPtr &comm)
	: FlightDevice(comm)
	, _active(false)
{
	_type = common::DeviceType::Raspi;
}

Raspi::~Raspi()
{
}

std::string Raspi::Name() const
{
	return _comm->connectParams().device + " " + _comm->connectParams().protocol;
}

bool Raspi::Start()
{
	if (_active)
		return true;

	// Initialize
	_active = true;
	_messageFactory.Init(shared_from_this());
	_telebox = data::TeleBox::Create();
	_telebox->Enable();
	_camera = onboard::Camera::Create();

	// Start reading from pixhawk
	_comm->Start([this](const ByteArray &data) { ReadData(data); });
	_processThread = std::thread([this]() { ProcessData(); });

	// Request data
	RequestData();
	StartHeartbeat();
	return true;
}

void Raspi::Stop()
{
	_active = false;
	_telebox->Disable();
	_camera.reset();

	if (_heartbeatThread.joinable())
		_heartbeatThread.join();

	if (_processThread.joinable())
		_processThread.join();

	_comm->Close();
}

bool Raspi::WaitHeartbeat()
{
	int heartbeatTimeout = 30;
	while (heartbeatTimeout-- >= 0)
	{
		if (_telebox->_hbTime > 0)
			return true;

		common::Sleep(1000);
	}

	return false;

	//return true;
}

void Raspi::StartCamera()
{
	// Cmd, Width, Height, Quality, Brightness
	SendCommand(MAV_CMD_VIDEO_START_CAPTURE, 640, 480, 70, 50);
	SetCamWork(true);
}

void Raspi::StopCamera()
{
	SendCommand(MAV_CMD_VIDEO_STOP_CAPTURE);
	SetCamWork(false);
}

void Raspi::StartHeartbeat()
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

		// First send init heartbeat
		mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);
		Send(message);

		// Then send usual heartbeat
		heartbeat.system_status = 0;
		mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);

		while (_active)
		{
			Send(message);
			common::Sleep(1000);
		}
	});
}

bool Raspi::RequestData()
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
		LOGE("[Raspi] Can't request data!");
		return false;
	}

	return true;
}

void Raspi::ReadData(const ByteArray &data)
{
	_mutexData.lock();
	std::copy(data.begin(), data.end(), std::back_inserter(_data));
	_mutexData.unlock();
}

bool Raspi::ProcessData()
{
	mavlink_message_t message;
	mavlink_status_t status;

	while (_active)
	{
		if (_data.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
			continue;
		}

		_mutexData.lock();
		ByteArray temp(std::move(_data));
		_mutexData.unlock();

		if (_camwork)
			_camera->ParseBytes(temp);

		for (size_t i = 0; i < temp.size(); ++i)
		{
			if (mavlink_parse_char(0, temp[i], &message, &status))
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
