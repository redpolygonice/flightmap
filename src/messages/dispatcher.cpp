#include "dispatcher.h"
#include "common/log.h"

namespace message
{

Dispatcher::Dispatcher()
	: _active(false)
{
}

Dispatcher::~Dispatcher()
{
	stop();
}

void Dispatcher::start()
{
	_active = true;
	_thread = std::thread([this]() { run(); });
}

void Dispatcher::stop()
{
	_active = false;
	_buffer.clear();

	if (_thread.joinable())
		_thread.join();
}

void Dispatcher::add(const MessagePtr &message)
{
	_buffer.push(message);
}

void Dispatcher::run()
{
	common::ThreadSafeList<MessagePtr>::iterator it;
	while (_active)
	{
//		for (it = _buffer.begin(); it != _buffer.end(); ++it)
//		{
//			MessagePtr message = *it;

//			if (message->state() == IMessage::State::Idle)
//			{
//				message->executeAsync();
//				LOGD("[Dispatcher] Message id: " << message->id());
//			}
//			else if (message->state() == IMessage::State::Done)
//			{
//				LOGD("[Dispatcher] Message Done id: " << message->id());
//				_buffer.remove(message);
//				break;
//			}
//			else if (message->state() == IMessage::State::Error)
//			{
//				LOGE("[Dispatcher] Message error, id: " << message->name() << ", error: " << message->error());
//				_buffer.remove(message);
//				break;
//			}
//		}

		MessagePtr message = _buffer.pop();
		if (message != nullptr)
		{
			message->executeAsync();
			//LOGD("[Dispatcher] Message id: " << message->id());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

}

