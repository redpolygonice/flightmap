#include "qtcoordinate.h"
#include "common/log.h"

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

double QtCoordinate::DistanceTo(const CoordinatePtr &point) const
{
	QGeoCoordinate coord(point->Lat(), point->Lon(), point->Alt());
	return _geoCoord.distanceTo(coord);
}

double QtCoordinate::AzimuthTo(const CoordinatePtr &point) const
{
	QGeoCoordinate coord(point->Lat(), point->Lon(), point->Alt());
	return _geoCoord.azimuthTo(coord);
}

CoordinatePtr QtCoordinate::AtDistanceAndAzimuth(double distance, double azimuth, double distanceUp) const
{
	QGeoCoordinate geoCoord = _geoCoord.atDistanceAndAzimuth(distance, azimuth, distanceUp);
	CoordinatePtr coordPtr = QtCoordinate::create(geoCoord.latitude(), geoCoord.longitude(), geoCoord.altitude());
	return coordPtr;
}

bool QtCoordinate::IsEqual(const CoordinatePtr &point) const
{
	return (100000 * Lat() == 100000 * point->Lat() &&
			100000 * Lon() == 100000 * point->Lon());

	//return std::fabs(lat() - point->lat()) <= 0.00001 &&
	//		std::fabs(lon() - point->lon()) <= 0.00001;
}

}
