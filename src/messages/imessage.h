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
	device::DevicePtr GetDevice() const { return _device; }
	uint32_t Id() const { return _id; }
	string Name() const { return _name; }
	string Error() const { return _error; }
	State State() const { return _state; }
	virtual void Execute() = 0;
	virtual void ExecuteAsync() { _future = std::async(std::launch::async, [this]() { Execute(); }); }
	virtual void Wait() { _future.wait(); }
};

typedef std::shared_ptr<IMessage> MessagePtr;

}

#endif // IMESSAGE_H
