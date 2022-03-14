#ifndef COMMBUILDER_H
#define COMMBUILDER_H

#include "common/types.h"
#include "icommunication.h"
#include "common/devicetypes.h"

namespace comms
{

// Communication builder (factory)
class CommBuilder
{
public:
	CommBuilder();
	CommunicationPtr create(const common::ConnectParams &params);
};

}

#endif // COMMBUILDER_H
