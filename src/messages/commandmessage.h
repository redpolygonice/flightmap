#ifndef COMMANDMESSAGE_H
#define COMMANDMESSAGE_H

#include "common/types.h"
#include "imessage.h"
#include "devices/idevice.h"

namespace message
{

class CommandMessage : public IMessage
{
public:
	CommandMessage(const device::DevicePtr &device);

public:
	static MessagePtr create(const device::DevicePtr &device) { return std::make_shared<CommandMessage>(device); }
	void Execute() override;

private:
	void StartStreaming();
	void StopStreaming();
};

}

#endif // COMMANDMESSAGE_H
