#ifndef GPSMESSAGE_H
#define GPSMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class GpsMessage : public IMessage
{
public:
	GpsMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<GpsMessage>(device); }
	void execute() override;
};

}

#endif // GPSMESSAGE_H
