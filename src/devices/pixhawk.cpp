#include "pixhawk.h"
#include "common/log.h"
#include "messages/dispatcher.h"

#include <mavlink/common/mavlink.h>

namespace device
{

Pixhawk::Pixhawk(const comms::CommunicationPtr &comm)
	: IDevice(comm)
	, _active(false)
{
	_type = common::DeviceType::Pixhawk;
}

device::Pixhawk::~Pixhawk()
{
}

std::string Pixhawk::name() const
{
	return _comm->connectParams().device + " " + _comm->connectParams().protocol;
}

bool Pixhawk::start()
{
	if (_active)
		return true;

	_active = true;
	_messageFactory.init(shared_from_this());
	_telebox = data::TeleBox::create();
	startHeartbeat();

	// Request telemetry data
	for (int i = 0; i < 5; ++i)
	{
		if (requestData())
			break;
	}

	// Start reading from pixhawk
	_comm->start([this](const ByteArray &data) { readData(data); });
	_processThread = std::thread([this]() { processData(); });
	return true;
}

void Pixhawk::stop()
{
	_active = false;

	if (_heartbeatThread.joinable())
		_heartbeatThread.join();

	if (_readThread.joinable())
		_readThread.join();

	if (_processThread.joinable())
		_processThread.join();

	_comm->close();
}

void Pixhawk::sendCommand(uint32_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
}

bool Pixhawk::sendMessage(mavlink_message_t &message)
{
	message.compid = MAV_COMP_ID_MISSIONPLANNER;
	message.sysid = _systemId;
	message.seq = _sequence;
	if (_comm->write(reinterpret_cast<const unsigned char *>(&message), sizeof(message)) > 0)
		return true;
	return false;
}

void Pixhawk::startHeartbeat()
{
	_heartbeatThread = std::thread([this]() {
		mavlink_message_t message;
		mavlink_heartbeat_t heartbeat;
		memset(&heartbeat, 0, sizeof(heartbeat));
		heartbeat.type = MAV_TYPE_GENERIC;
		heartbeat.autopilot = MAV_AUTOPILOT_GENERIC;
		mavlink_msg_heartbeat_encode(_systemId, _componentId, &message, &heartbeat);

		while (_active)
		{
			if (!_comm->write((const unsigned char *)&message, sizeof(message)))
			{
				LOGE("[Pixhawk] Can't send Heartbeat");
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	});
}

bool Pixhawk::requestData()
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

	bool result = false;
	mavlink_msg_request_data_stream_encode(_systemId, _componentId, &message, &data);
	if (_comm->write((const unsigned char *)&message, sizeof(message)))
	{
		result = true;
		LOG("[Pixhawk] requestData OK");
	}
	else
	{
		result = false;
		LOGE("[Pixhawk] requestData FAILED");
	}

	return result;
}

void Pixhawk::readData(const ByteArray &data)
{
	std::lock_guard<std::mutex> lock(_mutexData);
	_data.insert(_data.end(), data.begin(), data.end());
}

bool Pixhawk::processData()
{
	mavlink_message_t message;
	mavlink_status_t status;

	while (_active)
	{
		_mutexData.lock();
		bool empty = _data.empty();
		_mutexData.unlock();

		if (empty)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		_mutexData.lock();
		ByteArray temp(std::move(_data));
		_mutexData.unlock();

		for (size_t i = 0; i < temp.size(); ++i)
		{
			if (mavlink_parse_char(0, temp[i], &message, &status))
			{
				_sequence = message.seq;
				_targetSysid = message.sysid;
				_targetCompid = message.compid;

				message::MessagePtr messagePtr = _messageFactory.create(message);
				if (messagePtr != nullptr)
					message::Dispatcher::instance()->add(messagePtr);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return true;
}

}
