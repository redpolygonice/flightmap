#ifndef HEARTBEATMESSAGE_H
#define HEARTBEATMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class HeartbeatMessage : public IMessage
{
public:
	HeartbeatMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<HeartbeatMessage>(device); }
	void Execute() override;
};

}

#endif // HEARTBEATMESSAGE_H
