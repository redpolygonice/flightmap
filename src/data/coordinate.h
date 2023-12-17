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
	static CoordinatePtr Create() { return std::make_shared<Coordinate>(); }
	static CoordinatePtr Create(double lat, double lon, double alt = 0.0) { return std::make_shared<Coordinate>(lat, lon, alt); }
	double Lat() const override { return _lat; }
	double Lon() const override { return _lon; }
	double Alt() const override { return _alt; }
	void SetLat(double lat) override { _lat = lat; }
	void SetLon(double lon) override { _lon = lon;; }
	void SetAlt(double alt) override { _alt = alt; }
	double DistanceTo(const CoordinatePtr &point) const override;
	double AzimuthTo(const CoordinatePtr &point) const override;
	CoordinatePtr AtDistanceAndAzimuth(double distance, double azimuth, double distanceUp = 0.0) const override;
	bool IsEmpty() const override { return false; }
	bool IsEqual(const CoordinatePtr &point) const override;
};

}

#endif // COORDINATE_H
