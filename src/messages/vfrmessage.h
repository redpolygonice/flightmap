#ifndef VFRMESSAGE_H
#define VFRMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class VfrMessage : public IMessage
{
public:
	VfrMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<VfrMessage>(device); }
	void execute() override;
};

}

#endif // VFRMESSAGE_H
