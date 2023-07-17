#ifndef AUTOPILOTVERSIONMESSAGE_H
#define AUTOPILOTVERSIONMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class AutopilotVersionMessage : public IMessage
{
public:
	AutopilotVersionMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<AutopilotVersionMessage>(device); }
	void Execute() override;
};

}

#endif // AUTOPILOTVERSIONMESSAGE_H
