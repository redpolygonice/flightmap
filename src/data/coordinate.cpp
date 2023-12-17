#include "coordinate.h"

#include <math.h>

namespace data
{

Coordinate::Coordinate()
	: _lat(0.0)
	, _lon(0.0)
	, _alt(0.0)
{
}

Coordinate::Coordinate(double lat, double lon, double alt)
	: _lat(lat)
	, _lon(lon)
	, _alt(alt)
{
}

Coordinate::Coordinate(const Coordinate &other)
{
	*this = other;
}

void Coordinate::operator=(const Coordinate &other)
{
	_lat = other.Lat();
	_lon = other.Lon();
	_alt = other.Alt();
}

double Coordinate::DistanceTo(const CoordinatePtr &point) const
{
	double d = _lat * 0.017453292519943295;
	double num2 = _lon * 0.017453292519943295;
	double num3 = point->Lat() * 0.017453292519943295;
	double num4 = point->Lon() * 0.017453292519943295;
	double num5 = num4 - num2;
	double num6 = num3 - d;
	double num7 = std::pow(std::sin(num6 / 2.0), 2.0) + (std::cos(d) * std::cos(num3)) * std::pow(std::sin(num5 / 2.0), 2.0);
	double num8 = 2.0 * std::atan2(std::sqrt(num7), std::sqrt(1.0 - num7));
	return (6371 * num8) * 1000.0;
}

double Coordinate::AzimuthTo(const CoordinatePtr &point) const
{
//	QGeoCoordinate FlightGeoRoute::toGeoCoordinate(const QGeoCoordinate &origin, const QPointF &point)
//	{
//		auto distance = qSqrt(point.x()*point.x() + point.y()*point.y());
//		auto radianAngle = qAtan2(point.x(), point.y());
//		auto azimuth = qRadiansToDegrees(radianAngle < 0 ? radianAngle + 2*M_PI
//																									   : radianAngle);
//		return origin.atDistanceAndAzimuth(distance, azimuth);
//	}
	return 0.0;
}

CoordinatePtr Coordinate::AtDistanceAndAzimuth(double distance, double azimuth, double distanceUp) const
{
	CoordinatePtr coord = Coordinate::Create();
	return coord;
}

bool Coordinate::IsEqual(const CoordinatePtr &point) const
{
	return std::fabs(Lat() - point->Lat()) <= std::numeric_limits<double>::epsilon() &&
			std::fabs(Lon() - point->Lon()) <= std::numeric_limits<double>::epsilon();
}

}
