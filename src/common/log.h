#ifndef LOG_H
#define LOG_H

#include "types.h"
#include "threadsafequeue.h"

#include <fstream>
#include <sstream>

typedef std::function<void(const string &text)> LogFunction;

#define LOG(...) { std::stringstream __stream; __stream << __VA_ARGS__; common::log(__stream.str()); }
#define LOGW(...) { std::stringstream __stream; __stream << __VA_ARGS__; common::logw(__stream.str()); }
#define LOGE(...) { std::stringstream __stream; __stream << __VA_ARGS__; common::loge(__stream.str()); }
#define LOGC(...) { std::stringstream __stream; __stream << __VA_ARGS__; common::logc(__stream.str()); }
#define LOGD(...) { std::stringstream __stream; __stream << __VA_ARGS__; common::logd(__stream.str()); }

namespace common
{

class Log;
typedef std::shared_ptr<Log> LogPtr;

// Logging class
class Log
{
public:
	enum class Level
	{
		Critical,
		Error,
		Warning,
		Info,
		Debug
	};

private:
	static LogPtr _instance;
	bool _async = true;
	std::atomic_bool _active;
	Level _verb;
	ThreadSafeQueue<StringPtr> _queue;
	std::ofstream _stream;
	std::thread _thread;
	LogFunction _callback;

public:
	Log();
	~Log();

public:
	void SetVerbosity(Level level) { _verb = level; }
	void Write(const string &text, Log::Level level = Log::Level::Info);
	void WriteAsync(const string &text, Log::Level level = Log::Level::Info);
	void SetCallback(const LogFunction &callback) { _callback = callback; }

	static LogPtr Create()
	{
		if (_instance == nullptr)
			_instance = std::make_shared<Log>();
		return _instance;
	}

	static LogPtr Instance()
	{
		return Create();
	}

	static void Close()
	{
		_instance.reset();
		_instance = nullptr;
	}

	static void Put(const string &text, Log::Level level)
	{
		if (_instance->_async)
			_instance->WriteAsync(text, level);
		else
			_instance->Write(text, level);
	}

	static void SetVerb(Level level)
	{
		_instance->SetVerbosity(level);
	}

private:
	void Run();
	string CreateLine(const string &text, Log::Level level);
};

inline void log(const string &text, Log::Level level = Log::Level::Info) { Log::Put(text, level); }
inline void logw(const string &text) { log(text, Log::Level::Warning); }
inline void loge(const string &text) { log(text, Log::Level::Error); }
inline void logc(const string &text) { log(text, Log::Level::Critical); }
inline void logd(const string &text) { log(text, Log::Level::Debug); }

}

#endif // LOG_H
