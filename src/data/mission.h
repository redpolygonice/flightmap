#ifndef MISSION_H
#define MISSION_H

#include "common/types.h"
#include "imission.h"

namespace data
{

class Mission : public IMission
{
public:
	Mission();
	~Mission();
};

}

#endif // MISSION_H
