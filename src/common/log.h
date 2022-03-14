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
typedef std::shared_ptr<Log> LoggerPtr;

// Logging class
class Log
{
public:
	Log();
	~Log();
	enum class Level
	{
		Critical,
		Error,
		Warning,
		Info,
		Debug
	};

private:
	static LoggerPtr _instance;
	bool _async = true;
	std::atomic_bool _active;
	Level _verb;
	ThreadSafeQueue<StringPtr> _queue;
	std::ofstream _stream;
	std::thread _thread;
	LogFunction _callback;

public:
	void setVerbosity(Level level) { _verb = level; }
	void write(const string &text, Log::Level level = Log::Level::Info);
	void writeAsync(const string &text, Log::Level level = Log::Level::Info);
	void setCallback(const LogFunction &callback) { _callback = callback; }

	static LoggerPtr create()
	{
		if (_instance == nullptr)
			_instance = std::make_shared<Log>();
		return _instance;
	}

	static void close()
	{
		_instance.reset();
		_instance = nullptr;
	}

	static void put(const string &text, Log::Level level)
	{
		if (_instance->_async)
			_instance->writeAsync(text, level);
		else
			_instance->write(text, level);
	}

	static void setVerb(Level level)
	{
		_instance->setVerbosity(level);
	}

private:
	void run();
	string createLine(const string &text, Log::Level level);
};

inline void log(const string &text, Log::Level level = Log::Level::Info) { Log::put(text, level); }
inline void logw(const string &text) { log(text, Log::Level::Warning); }
inline void loge(const string &text) { log(text, Log::Level::Error); }
inline void logc(const string &text) { log(text, Log::Level::Critical); }
inline void logd(const string &text) { log(text, Log::Level::Debug); }

}

#endif // LOG_H
