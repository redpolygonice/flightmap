#ifndef ICOORDINATE_H
#define ICOORDINATE_H

#include "common/types.h"

namespace data
{

class ICoordinate;
typedef std::shared_ptr<ICoordinate> CoordinatePtr;

// Coordinate interface
class ICoordinate
{
protected:
	bool _basePoint;

public:
	ICoordinate() : _basePoint(false) {}
	virtual ~ICoordinate() {}

public:
	virtual void SetBasePoint(bool arg = true) { _basePoint = arg; }
	virtual bool IsBasePoint() const { return _basePoint; }
	virtual double Lat() const = 0;
	virtual double Lon() const = 0;
	virtual double Alt() const = 0;
	virtual void SetLat(double lat) = 0;
	virtual void SetLon(double lon) = 0;
	virtual void SetAlt(double alt) = 0;
	virtual double DistanceTo(const CoordinatePtr &point) const = 0;
	virtual double AzimuthTo(const CoordinatePtr &point) const = 0;
	virtual CoordinatePtr AtDistanceAndAzimuth(double distance, double azimuth, double distanceUp = 0.0) const = 0;
	virtual bool IsEmpty() const = 0;
	virtual bool IsEqual(const CoordinatePtr &point) const { return false; }
};

typedef std::vector<CoordinatePtr> CoordinateList;
typedef std::vector<CoordinatePtr>::iterator CoordinateListIterator;
typedef std::vector<CoordinatePtr>::const_iterator CoordinateListConstIterator;

}

#endif // ICOORDINATE_H
