#include "grid.h"
#include "common/log.h"
#include "common/common.h"
#include "main/broker.h"
#include "qtcoordinate.h"

namespace data
{

Grid::Grid()
{
}

Grid::~Grid()
{
}

bool Grid::create(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params)
{
	common::AnyMap missionParams = params;
	if (missionParams.get<bool>("polygon"))
		return createPolygon(markers, mission, params);
	else
		return createPoints(markers, mission, params);
}

bool Grid::createPoints(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params)
{
	if (markers.size() <= 1)
	{
		LOGW("Too small coordinate count!");
		return false;
	}

	common::AnyMap missionParams = params;
	double step = missionParams.get<int>("missionStep");
	bool altCheck = missionParams.get<bool>("altCheck");
	int altCheckValue = missionParams.get<int>("alt");
	common::AltType altType = static_cast<common::AltType>(missionParams.get<int>("altType"));

	auto computeAlt = [&](const CoordinatePtr &coord) {
		if (altCheck)
		{
			float alt = 50.0;
			if (altType == common::AltType::Absolute || altType == common::AltType::Terrain)
				alt = altCheckValue;
			else if (altType == common::AltType::Relative)
				alt = core::Broker::instance()->getAltitude(coord->lat(), coord->lon()) + altCheckValue;
			coord->setAlt(alt);
		}
	};

	// Home point
	CoordinatePtr homePoint = QtCoordinate::create(missionParams.get<double>("homeLat"), missionParams.get<double>("homeLon"));
	computeAlt(homePoint);
	homePoint->setBasePoint(true);
	mission.push_back(homePoint);

	for (int i = 0; i < markers.size() - 1; ++i)
	{
		CoordinatePtr currentPoint = markers[i];
		CoordinatePtr nextPoint = markers[i + 1];

		double distance = currentPoint->distanceTo(nextPoint);
		double azimuth = currentPoint->azimuthTo(nextPoint);
		int stepCount = distance / step;

		computeAlt(currentPoint);
		currentPoint->setBasePoint(true);
		mission.push_back(currentPoint);

		if (stepCount > 1)
		{
			CoordinatePtr point = currentPoint;
			for (int j = 0; j < stepCount; ++j)
			{
				point = point->atDistanceAndAzimuth(step, azimuth);

				// Check distance at last point
				if (j == stepCount - 1)
				{
					double dist = point->distanceTo(nextPoint);
					if (dist < step / 2)
						break;
				}

				computeAlt(point);
				mission.push_back(point);
			}
		}
	}

	// Last point
	CoordinatePtr lastPoint = markers[markers.size() - 1];
	computeAlt(lastPoint);
	lastPoint->setBasePoint(true);
	mission.push_back(lastPoint);

	// Line from last point to home
	mission.push_back(homePoint);

	return true;
}

bool Grid::createPolygon(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params)
{
	return false;
}

}
