#ifndef ATTITUDEMESSAGE_H
#define ATTITUDEMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class AttitudeMessage : public IMessage
{
public:
	AttitudeMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<AttitudeMessage>(device); }
	void Execute() override;
};

}

#endif // ATTITUDEMESSAGE_H
