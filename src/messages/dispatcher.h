#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "common/types.h"
#include "common/singleton.h"
#include "messages/imessage.h"
#include "common/threadsafelist.h"

namespace message
{

class Dispatcher : public Singleton<Dispatcher>
{
	friend class Singleton<Dispatcher>;

private:
	std::atomic_bool _active;
	std::thread _thread;
	common::ThreadSafeList<MessagePtr> _buffer;

private:
	void Run();

private:
	Dispatcher();

public:
	~Dispatcher();

public:
	void Start();
	void Stop();
	void Add(const MessagePtr &message);
};

}

#endif // DISPATCHER_H
