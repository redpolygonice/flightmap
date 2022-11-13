#ifndef COORDINATE_H
#define COORDINATE_H

#include "common/types.h"
#include "icoordinate.h"

namespace data
{

// GPS coordinate object
class Coordinate : public ICoordinate
{
private:
	double _lat;
	double _lon;
	double _alt;

public:
	Coordinate();
	Coordinate(double lat, double lon, double alt = 0.0);
	Coordinate(const Coordinate &other);
	void operator= (const Coordinate &other);

public:
	static CoordinatePtr create() { return std::make_shared<Coordinate>(); }
	static CoordinatePtr create(double lat, double lon, double alt = 0.0) { return std::make_shared<Coordinate>(lat, lon, alt); }
	double lat() const override { return _lat; }
	double lon() const override { return _lon; }
	double alt() const override { return _alt; }
	void setLat(double lat) override { _lat = lat; }
	void setLon(double lon) override { _lon = lon;; }
	void setAlt(double alt) override { _alt = alt; }
	double distanceTo(const CoordinatePtr &point) const override;
	double azimuthTo(const CoordinatePtr &point) const override;
	CoordinatePtr atDistanceAndAzimuth(double distance, double azimuth, double distanceUp = 0.0) const override;
	bool isEmpty() const override { return false; }
};

}

#endif // COORDINATE_H
