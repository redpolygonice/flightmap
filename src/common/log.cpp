#include "log.h"
#include "common.h"

namespace common
{

LoggerPtr Log::_instance = nullptr;

Log::Log()
	: _active(false)
	, _verb(Level::Info)
	, _callback(nullptr)
{
	string logName = "log-" + currentTime() + ".txt";
	_stream.open(logName, std::ios_base::out | std::ios_base::trunc);
	if (!_stream.is_open())
		return;

	_active = true;
	_stream << "[" << currentTimeMs() << "] " << "Start"  << std::endl;

	if (_async)
	{
		_thread = std::thread([this]() { run(); });
		_thread.detach();
	}
}

Log::~Log()
{
	while (!_queue.isEmpty())
		sleep(5);

	_active = false;
	if (_stream.is_open())
	{
		_stream << "[" << currentTimeMs() << "] " << "Quit"  << std::endl;
		_stream.close();
	}
}

void Log::write(const std::string &text, Log::Level level)
{
	if (level > _verb)
		return;

	string line = createLine(text, level);
	std::cout << line << std::endl;
	_stream << line << std::endl;
	_stream.flush();

	if (_callback != nullptr)
		_callback(line);
}

void Log::writeAsync(const std::string &text, Log::Level level)
{
	if (level > _verb)
		return;

	_queue.push(std::make_shared<string>(createLine(text, level)));
}

std::string Log::createLine(const std::string &text, Log::Level level)
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

void Log::run()
{
	while (_active)
	{
		StringPtr text = _queue.pop();
		if (text != nullptr)
		{
			std::cout << *text << std::endl;
			_stream << *text << std::endl;
			_stream.flush();

			if (_callback != nullptr)
				_callback(*text);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

}
