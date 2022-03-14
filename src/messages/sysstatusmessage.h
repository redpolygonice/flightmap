#ifndef SYSSTATUSMESSAGE_H
#define SYSSTATUSMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class SysStatusMessage : public IMessage
{
public:
	SysStatusMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<SysStatusMessage>(device); }
	void execute() override;
};

}

#endif // SYSSTATUSMESSAGE_H
