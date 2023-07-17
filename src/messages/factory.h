#ifndef FACTORY_H
#define FACTORY_H

#include "common/types.h"
#include "messages/imessage.h"
#include "devices/idevice.h"

#include <mavlink/common/mavlink.h>

namespace message
{

// Message factory
class Factory
{
private:
	device::DevicePtr _device;

public:
	Factory();

public:
	void Init(const device::DevicePtr &device);
	MessagePtr Create(const mavlink_message_t &mavlink);

private:
	void InitDefaultParams(const MessagePtr &message, const mavlink_message_t &mavlink, const string &name);
};

}

#endif // FACTORY_H
