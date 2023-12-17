#ifndef GRID_H
#define GRID_H

#include "common/types.h"
#include "igrid.h"

namespace data
{

struct RectDouble
{
	double left;
	double top;
	double right;
	double bottom;

	RectDouble(double x, double y, double cx, double cy)
	{
		left = x;
		top = y;
		right = cx;
		bottom = cy;
	}
};

// Mission grid
class Grid : public IGrid
{
private:
	common::AnyMap _params;
	CoordinateList _markers;

public:
	Grid();
	virtual ~Grid();

public:
	static GridPtr Create() { return std::make_shared<Grid>(); }
	bool Create(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params) override;
	bool LoadFile(const string &fileName, CoordinateList &points, common::AnyMap &params) override;
	bool SaveFile(const string &fileName, const CoordinateList &points, const common::AnyMap &params) override;

private:
	bool CreatePoints(CoordinateList &mission);
	bool CreatePolygon(CoordinateList &mission);
	void CreateVerticalPoints(CoordinateList &points);
	void CreateHorizontalPoints(CoordinateList &points);
	RectDouble CreateBoundedRect(const CoordinateList &markers) const;
	CoordinatePtr FindIntersectionPoint(const CoordinatePtr &p1, const CoordinatePtr &p2, const CoordinatePtr &p3, const CoordinatePtr &p4) const;
	void CreatePoints(const CoordinatePtr &first, const CoordinatePtr &second, CoordinateList &points);
	void ComputeAlt(const CoordinatePtr &coord) const;
};

}

#endif // GRID_H
