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
	double lat() const override { return _geoCoord.latitude(); }
	double lon() const override { return _geoCoord.longitude(); }
	double alt() const override { return _geoCoord.altitude(); }
	void setLat(double lat) override { _geoCoord.setLatitude(lat); }
	void setLon(double lon) override { _geoCoord.setLongitude(lon); }
	void setAlt(double alt) override { _geoCoord.setAltitude(alt); }
	double distanceTo(const CoordinatePtr &point) const override;
	double azimuthTo(const CoordinatePtr &point) const override;
	CoordinatePtr atDistanceAndAzimuth(double distance, double azimuth, double distanceUp = 0.0) const override;
	bool isEmpty() const override { return _geoCoord.isValid(); }
};

}

#endif // QTCOORDINATE_H
