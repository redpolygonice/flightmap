#ifndef GRID_H
#define GRID_H

#include "common/types.h"
#include "igrid.h"

namespace data
{

// Mission grid
class Grid : public IGrid
{
public:
	Grid();
	virtual ~Grid();

public:
	static GridPtr create() { return std::make_shared<Grid>(); }
	bool create(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) override;

private:
	bool createPoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params);
	bool createPolygon(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params);
};

}

#endif // GRID_H
