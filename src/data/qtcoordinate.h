#ifndef QTCOORDINATE_H
#define QTCOORDINATE_H

#include "common/types.h"
#include "icoordinate.h"

#include <QGeoCoordinate>

namespace data
{

// Qt geo coordinate object
class QtCoordinate : public ICoordinate
{
private:
	QGeoCoordinate _geoCoord;

public:
	QtCoordinate();
	QtCoordinate(double lat, double lon, double alt = 0.0);
	virtual ~QtCoordinate();

public:
	static CoordinatePtr create() { return std::make_shared<QtCoordinate>(); }
	static CoordinatePtr create(double lat, double lon, double alt = 0.0) { return std::make_shared<QtCoordinate>(lat, lon, alt); }
	double Lat() const override { return _geoCoord.latitude(); }
	double Lon() const override { return _geoCoord.longitude(); }
	double Alt() const override { return _geoCoord.altitude(); }
	void SetLat(double lat) override { _geoCoord.setLatitude(lat); }
	void SetLon(double lon) override { _geoCoord.setLongitude(lon); }
	void SetAlt(double alt) override { _geoCoord.setAltitude(alt); }
	double DistanceTo(const CoordinatePtr &point) const override;
	double AzimuthTo(const CoordinatePtr &point) const override;
	CoordinatePtr AtDistanceAndAzimuth(double distance, double azimuth, double distanceUp = 0.0) const override;
	bool IsEmpty() const override { return _geoCoord.isValid(); }
	bool IsEqual(const CoordinatePtr &point) const override;
};

}

#endif // QTCOORDINATE_H
