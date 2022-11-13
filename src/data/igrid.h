#ifndef IGRID_H
#define IGRID_H

#include "common/types.h"
#include "data/icoordinate.h"
#include "common/anymap.h"

namespace data
{

// Mission grid interface
class IGrid
{
public:
	IGrid() {}
	virtual ~IGrid() {}

public:
	virtual bool create(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) = 0;
};

typedef std::shared_ptr<IGrid> GridPtr;

}

#endif // IGRID_H
