#include "log.h"
#include "common.h"

namespace common
{

LogPtr Log::_instance = nullptr;

Log::Log()
	: _active(false)
	, _verb(Level::Info)
	, _callback(nullptr)
{
	string logName = "log-" + CurrentTime() + ".txt";
	_stream.open(logName, std::ios_base::out | std::ios_base::trunc);
	if (!_stream.is_open())
		return;

	_active = true;
	_stream << "[" << currentTimeMs() << "] " << "Start"  << std::endl;

	if (_async)
	{
		_thread = std::thread([this]() { Run(); });
		_thread.detach();
	}
}

Log::~Log()
{
	while (!_queue.Empty())
		Sleep(5);

	_active = false;
	if (_stream.is_open())
	{
		_stream << "[" << currentTimeMs() << "] " << "Quit"  << std::endl;
		_stream.close();
	}
}

void Log::Write(const std::string &text, Log::Level level)
{
	if (level > _verb)
		return;

	string line = CreateLine(text, level);
	std::cout << line << std::endl;
	_stream << line << std::endl;
	_stream.flush();

	if (_callback != nullptr)
		_callback(line);
}

void Log::WriteAsync(const std::string &text, Log::Level level)
{
	if (level > _verb)
		return;

	StringPtr stringPtr = std::make_unique<string>(CreateLine(text, level));
	_queue.Push(std::move(stringPtr));
}

std::string Log::CreateLine(const std::string &text, Log::Level level)
{
	string prefix = "";
	switch (level)
	{
	case Log::Level::Info:
		prefix = "Info!    ";
		break;
	case Log::Level::Warning:
		prefix = "Warning! ";
		break;
	case Log::Level::Error:
		prefix = "Error!   ";
		break;
	case Log::Level::Critical:
		prefix = "Critical!!! ";
		break;
	case Log::Level::Debug:
		prefix = "Debug!   ";
		break;
	}

	string line = "[" + currentTimeMs() + "] " + prefix + text;
	return line;
}

void Log::Run()
{
	while (_active)
	{
		StringPtr text;
		_queue.Pop(std::move(text));

		if (text != nullptr)
		{
			std::cout << *text << std::endl;
			_stream << *text << std::endl;
			_stream.flush();

			if (_callback != nullptr)
				_callback(*text);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

}
