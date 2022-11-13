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
	void setBasePoint(bool arg = true) { _basePoint = arg; }
	bool isBasePoint() const { return _basePoint; }
	virtual double lat() const = 0;
	virtual double lon() const = 0;
	virtual double alt() const = 0;
	virtual void setLat(double lat) = 0;
	virtual void setLon(double lon) = 0;
	virtual void setAlt(double alt) = 0;
	virtual double distanceTo(const CoordinatePtr &point) const = 0;
	virtual double azimuthTo(const CoordinatePtr &point) const = 0;
	virtual CoordinatePtr atDistanceAndAzimuth(double distance, double azimuth, double distanceUp = 0.0) const = 0;
	virtual bool isEmpty() const = 0;
};

typedef std::vector<CoordinatePtr> CoordinateList;
typedef std::vector<CoordinatePtr>::iterator CoordinateListIterator;
typedef std::vector<CoordinatePtr>::const_iterator CoordinateListConstIterator;

}

#endif // ICOORDINATE_H
