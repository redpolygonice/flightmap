#include "qtcoordinate.h"

namespace data
{

QtCoordinate::QtCoordinate()
{
}

QtCoordinate::QtCoordinate(double lat, double lon, double alt)
	: _geoCoord(lat, lon, alt)
{
}

data::QtCoordinate::~QtCoordinate()
{
}

double QtCoordinate::distanceTo(const CoordinatePtr &point) const
{
	QGeoCoordinate coord(point->lat(), point->lon(), point->alt());
	return _geoCoord.distanceTo(coord);
}

double QtCoordinate::azimuthTo(const CoordinatePtr &point) const
{
	QGeoCoordinate coord(point->lat(), point->lon(), point->alt());
	return _geoCoord.azimuthTo(coord);
}

CoordinatePtr QtCoordinate::atDistanceAndAzimuth(double distance, double azimuth, double distanceUp) const
{
	QGeoCoordinate geoCoord = _geoCoord.atDistanceAndAzimuth(distance, azimuth, distanceUp);
	CoordinatePtr coordPtr = QtCoordinate::create(geoCoord.latitude(), geoCoord.longitude(), geoCoord.altitude());
	return coordPtr;
}

}
