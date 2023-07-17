#ifndef STATUSTEXTMESSAGE_H
#define STATUSTEXTMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class StatusTextMessage : public IMessage
{
public:
	StatusTextMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<StatusTextMessage>(device); }
	void Execute() override;
};

}

#endif // STATUSTEXTMESSAGE_H
