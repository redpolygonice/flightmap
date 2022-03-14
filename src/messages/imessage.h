#ifndef IMESSAGE_H
#define IMESSAGE_H

#include "common/types.h"
#include "common/devicetypes.h"
#include "devices/idevice.h"

namespace message
{

// Message interface
class IMessage
{
	friend class Factory;

public:
	enum class Direction
	{
		FromDevice,
		ToDevice
	};

	enum class State
	{
		Idle,
		Progress,
		Done,
		Error
	};

protected:
	device::DevicePtr _device;
	uint32_t _id;
	unsigned char _sequence;
	string _name;
	Direction _direction;
	std::atomic<State> _state;
	AnyMapType _params;
	string _error;
	std::future<void> _future;

public:
	IMessage(const device::DevicePtr &device) : _device(device) {}
	virtual ~IMessage() {}

public:
	device::DevicePtr device() const { return _device; }
	uint32_t id() const { return _id; }
	string name() const { return _name; }
	string error() const { return _error; }
	State state() const { return _state; }
	virtual void execute() = 0;
	virtual void executeAsync() { _future = std::async(std::launch::async, [this]() { execute(); }); }
	virtual void wait() { _future.wait(); }
};

typedef std::shared_ptr<IMessage> MessagePtr;

}

#endif // IMESSAGE_H
