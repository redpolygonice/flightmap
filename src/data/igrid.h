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
	virtual bool Create(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) = 0;
	virtual bool LoadFile(const string &fileName, CoordinateList &points, common::AnyMap &params) = 0;
	virtual bool SaveFile(const string &fileName, const CoordinateList &points, const common::AnyMap &params) = 0;
};

typedef std::shared_ptr<IGrid> GridPtr;

}

#endif // IGRID_H
