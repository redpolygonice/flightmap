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
	Stop();
}

void Dispatcher::Start()
{
	_active = true;
	_thread = std::thread([this]() { Run(); });
}

void Dispatcher::Stop()
{
	_active = false;
	_buffer.Clear();

	if (_thread.joinable())
		_thread.join();
}

void Dispatcher::Add(const MessagePtr &message)
{
	_buffer.Push(message);
}

void Dispatcher::Run()
{
	while (_active)
	{
		MessagePtr message = _buffer.Pop();
		if (message != nullptr)
			message->Execute();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

}

