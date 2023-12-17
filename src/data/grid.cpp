#include "grid.h"
#include "common/log.h"
#include "common/common.h"
#include "data/icoordinate.h"
#include "main/broker.h"
#include "qtcoordinate.h"
#include <algorithm>
#include <cstddef>
#include <vector>

namespace data
{

Grid::Grid()
{
	std::setlocale(LC_ALL, "en_US.UTF-8");
}

Grid::~Grid()
{
}

bool Grid::Create(const CoordinateList &markers, CoordinateList &mission, const common::AnyMap &params)
{
	if (markers.size() <= 1)
	{
		LOGW("Too small coordinate count!");
		return false;
	}

	if (_params.Get<bool>("polygon") && !params.Get<bool>("horizStepCheck") && !params.Get<bool>("vertStepCheck"))
	{
		LOGW("Horizontal and Vertical routes not defined!");
		return false;
	}

	_params = params;
	_markers = markers;

	if (_params.Get<bool>("polygon"))
		return CreatePolygon(mission);
	else
		return CreatePoints(mission);
}

bool Grid::LoadFile(const std::string &fileName, CoordinateList &points, common::AnyMap &params)
{
	points.clear();

	std::ifstream ifs(fileName, std::ios::in);
	if (!ifs.is_open())
	{
		LOGE("Can't load mission file!");
		return false;
	}

	// Load mission parameters
	string line;
	std::getline(ifs, line);
	StringList data = common::SplitString(line, ";");
	if (data.size() != 9)
	{
		LOGE("Incorrect size of parameters!");
		return false;
	}

	params.Set("polygon", std::stoi(data[0]) == 1 ? true : false);
	params.Set("alt", std::stoi(data[1]));
	params.Set("altCheck", std::stoi(data[2]) == 1 ? true : false);
	params.Set("altType", std::stoi(data[3]));
	params.Set("horizStepCheck", std::stoi(data[4]) == 1 ? true : false);
	params.Set("horizStep", std::stoi(data[5]));
	params.Set("vertStepCheck", std::stoi(data[6]) == 1 ? true : false);
	params.Set("vertStep", std::stoi(data[7]));
	params.Set("missionStep", std::stoi(data[8]));

	// Load mission points
	while(std::getline(ifs, line))
	{
		StringList data = common::SplitString(line, ";");
		double lat = std::stod(data[2]);
		double lon = std::stod(data[3]);
		double alt = std::stod(data[4]);

		CoordinatePtr point = QtCoordinate::create(lat, lon, alt);
		points.push_back(point);
	}

	ifs.close();
	return true;
}

bool Grid::SaveFile(const std::string &fileName, const CoordinateList &points, const common::AnyMap &params)
{
	std::ofstream ofs(fileName, std::ios_base::out | std::ios_base::trunc);
	if (!ofs.is_open())
	{
		LOGE("Can't create mission file!");
		return false;
	}

	// Save mission parameters
	ofs << params.Get<bool>("polygon") << ";";
	ofs << params.Get<int>("alt") << ";";
	ofs << params.Get<bool>("altCheck") << ";";
	ofs << params.Get<int>("altType") << ";";
	ofs << params.Get<bool>("horizStepCheck") << ";";
	ofs << params.Get<int>("horizStep") << ";";
	ofs << params.Get<bool>("vertStepCheck") << ";";
	ofs << params.Get<int>("vertStep") << ";";
	ofs << params.Get<int>("missionStep");
	ofs << std::endl;

	// Save mission points
	int cmd = MAV_CMD_NAV_WAYPOINT;
	for (int i = 0; i < points.size(); ++i)
	{
		if (points[i]->IsBasePoint())
			continue;

		ofs << (i + 1) << ";" << cmd << ";" << points[i]->Lat() << ";" << points[i]->Lon() <<  ";" << points[i]->Alt() << std::endl;
	}

	ofs.close();
	return true;
}

bool Grid::CreatePoints(CoordinateList &mission)
{
	// Home point
	CoordinatePtr homePoint = QtCoordinate::create(_params.Get<double>("homeLat"), _params.Get<double>("homeLon"));
	ComputeAlt(homePoint);
	mission.push_back(homePoint);

	for (int i = 0; i < _markers.size() - 1; ++i)
	{
		CoordinatePtr currentPoint = _markers[i];
		CoordinatePtr nextPoint = _markers[i + 1];

		ComputeAlt(currentPoint);
		mission.push_back(currentPoint);
		CreatePoints(currentPoint, nextPoint, mission);
	}

	// Last point
	CoordinatePtr lastPoint = _markers[_markers.size() - 1];
	ComputeAlt(lastPoint);
	mission.push_back(lastPoint);

	// Line from last point to home
	mission.push_back(homePoint);

	return true;
}

bool Grid::CreatePolygon(CoordinateList &mission)
{
	// Home point
	CoordinatePtr homePoint = QtCoordinate::create(_params.Get<double>("homeLat"), _params.Get<double>("homeLon"));
	ComputeAlt(homePoint);
	mission.push_back(homePoint);

	// Last point - first point
	_markers.push_back(_markers[0]);

	// Create grid lines
	CoordinateList points;
	if (_params.Get<bool>("vertStepCheck"))
		CreateVerticalPoints(mission);

	if (_params.Get<bool>("horizStepCheck"))
		CreateHorizontalPoints(mission);

	// Line from last point to home
	mission.push_back(homePoint);

	// Add base points to mission
	for (CoordinatePtr &point : _markers)
	{
		point->SetBasePoint();
		mission.push_back(point);
	}

	return true;
}

void Grid::CreateVerticalPoints(CoordinateList &points)
{
	// Create bounded rectangle
	RectDouble rect = CreateBoundedRect(_markers);
	int vertStep = _params.Get<int>("vertStep");

	// Check distance in meters
	CoordinatePtr startPoint = QtCoordinate::create(rect.top, rect.left);
	CoordinatePtr endPoint = QtCoordinate::create(rect.top, rect.right);

	double rectDistance = startPoint->DistanceTo(endPoint);
	if (vertStep >= rectDistance)
		return;

	const double azimuth = 90.0;
	int stepCount = rectDistance / vertStep;

	CoordinatePtr point = startPoint;
	std::vector<CoordinateList> intersectionPoints;

	// Create intersection points
	for (int i = 0; i < stepCount; ++i)
	{
		point = point->AtDistanceAndAzimuth(vertStep, azimuth);
		CoordinatePtr topPoint = QtCoordinate::create(rect.top, point->Lon());
		CoordinatePtr bottomPoint = QtCoordinate::create(rect.bottom, point->Lon());
		CoordinateList pointList;

		for (int j = 0; j < _markers.size() - 1; ++j)
		{
			CoordinatePtr foundPoint = FindIntersectionPoint(topPoint, bottomPoint, _markers[j], _markers[j + 1]);
			if (foundPoint != nullptr)
				pointList.push_back(foundPoint);
		}

		intersectionPoints.push_back(pointList);
	}

	// Connect points
	bool top = false;
	for (int i = 0; i < intersectionPoints.size(); ++i)
	{
		CoordinateList currentPoints = intersectionPoints[i];
		CoordinatePtr topPoint = nullptr;
		CoordinatePtr bottomPoint = nullptr;

		if (i == 0)
		{
			topPoint = currentPoints[0];
			bottomPoint = currentPoints[1];
		}
		else if (!top)
		{
			topPoint = (currentPoints[1]->Lat() < currentPoints[0]->Lat() ? currentPoints[1] : currentPoints[0]);
			bottomPoint = (currentPoints[0]->Lat() > currentPoints[1]->Lat() ? currentPoints[0] : currentPoints[1]);
			top = true;
		}
		else
		{
			topPoint = (currentPoints[0]->Lat() > currentPoints[1]->Lat() ? currentPoints[0] : currentPoints[1]);
			bottomPoint = (currentPoints[1]->Lat() < currentPoints[0]->Lat() ? currentPoints[1] : currentPoints[0]);
			top = false;
		}

		ComputeAlt(topPoint);
		ComputeAlt(bottomPoint);

		points.push_back(topPoint);
		CreatePoints(topPoint, bottomPoint, points);
		points.push_back(bottomPoint);
	}
}

void Grid::CreateHorizontalPoints(CoordinateList &points)
{
	// Create bounded rectangle
	RectDouble rect = CreateBoundedRect(_markers);
	int horizStep = _params.Get<int>("horizStep");

	// Check distance in meters
	CoordinatePtr startPoint = QtCoordinate::create(rect.top, rect.left);
	CoordinatePtr endPoint = QtCoordinate::create(rect.bottom, rect.left);

	double rectDistance = startPoint->DistanceTo(endPoint);
	if (horizStep >= rectDistance)
		return;

	const double azimuth = 180.0;
	int stepCount = rectDistance / horizStep;

	CoordinatePtr point = startPoint;
	std::vector<CoordinateList> intersectionPoints;

	// Create intersection points
	for (int i = 0; i < stepCount; ++i)
	{
		point = point->AtDistanceAndAzimuth(horizStep, azimuth);
		CoordinatePtr leftPoint = QtCoordinate::create(point->Lat(), rect.left);
		CoordinatePtr rightPoint = QtCoordinate::create(point->Lat(), rect.right);
		CoordinateList pointList;

		for (int j = 0; j < _markers.size() - 1; ++j)
		{
			CoordinatePtr foundPoint = FindIntersectionPoint(leftPoint, rightPoint, _markers[j], _markers[j + 1]);
			if (foundPoint != nullptr)
				pointList.push_back(foundPoint);
		}

		intersectionPoints.push_back(pointList);
	}

	// Connect points
	bool left = false;
	for (int i = 0; i < intersectionPoints.size(); ++i)
	{
		CoordinateList currentPoints = intersectionPoints[i];
		CoordinatePtr leftPoint = nullptr;
		CoordinatePtr rightPoint = nullptr;

		if (i == 0)
		{
			leftPoint = currentPoints[0];
			rightPoint = currentPoints[1];
		}
		else if (!left)
		{
			leftPoint = (currentPoints[1]->Lon() > currentPoints[0]->Lon() ? currentPoints[1] : currentPoints[0]);
			rightPoint = (currentPoints[0]->Lon() < currentPoints[1]->Lon() ? currentPoints[0] : currentPoints[1]);
			left = true;
		}
		else
		{
			leftPoint = (currentPoints[0]->Lon() < currentPoints[1]->Lon() ? currentPoints[0] : currentPoints[1]);
			rightPoint = (currentPoints[1]->Lon() > currentPoints[0]->Lon() ? currentPoints[1] : currentPoints[0]);
			left = false;
		}

		ComputeAlt(leftPoint);
		ComputeAlt(rightPoint);

		points.push_back(leftPoint);
		CreatePoints(leftPoint, rightPoint, points);
		points.push_back(rightPoint);
	}
}

RectDouble Grid::CreateBoundedRect(const CoordinateList &markers) const
{
	double right = 0.0;
	for (const CoordinatePtr &point : markers)
	{
		if (point->Lon() > right)
			right = point->Lon();
	}

	double left = right;
	for (const CoordinatePtr &point : markers)
	{
		if (point->Lon() < left)
			left = point->Lon();
	}

	double top = 0.0;
	for (const CoordinatePtr &point : markers)
	{
		if (point->Lat() > top)
			top = point->Lat();
	}

	double bottom = top;
	for (const CoordinatePtr &point : markers)
	{
		if (point->Lat() < bottom)
			bottom = point->Lat();
	}

	return RectDouble(left, top, right, bottom);
}

CoordinatePtr Grid::FindIntersectionPoint(const CoordinatePtr &p1, const CoordinatePtr &p2, const CoordinatePtr &p3, const CoordinatePtr &p4) const
{
	double d = (p1->Lon() - p2->Lon()) * (p4->Lat() - p3->Lat()) - (p1->Lat() - p2->Lat()) * (p4->Lon() - p3->Lon());
	double da = (p1->Lon() - p3->Lon()) * (p4->Lat() - p3->Lat()) - (p1->Lat() - p3->Lat()) * (p4->Lon() - p3->Lon());
	double db = (p1->Lon() - p2->Lon()) * (p1->Lat() - p3->Lat()) - (p1->Lat() - p2->Lat()) * (p1->Lon() - p3->Lon());

	double ta = da / d;
	double tb = db / d;

	if (ta >= 0 && ta <= 1 && tb >= 0 && tb <= 1)
	{
		double dx = p1->Lon() + ta * (p2->Lon() - p1->Lon());
		double dy = p1->Lat() + ta * (p2->Lat() - p1->Lat());
		CoordinatePtr point = QtCoordinate::create(dy, dx);
		return point;
	}

	return nullptr;
}

void Grid::CreatePoints(const CoordinatePtr &first, const CoordinatePtr &second, CoordinateList &points)
{
	double distance = first->DistanceTo(second);
	double azimuth = first->AzimuthTo(second);
	int step = _params.Get<int>("missionStep");
	int stepCount = distance / step;

	if (stepCount > 1)
	{
		CoordinatePtr point = first;
		for (int j = 0; j < stepCount; ++j)
		{
			point = point->AtDistanceAndAzimuth(step, azimuth);

			// Check distance at last point
			if (j == stepCount - 1)
			{
				double dist = point->DistanceTo(second);
				if (dist < step / 2)
					break;
			}

			ComputeAlt(point);
			points.push_back(point);
		}
	}
}

void Grid::ComputeAlt(const CoordinatePtr &coord) const
{
	float alt = core::Broker::instance()->ComputeAlt(coord->Lat(), coord->Lon(), _params.Get<bool>("altCheck"), _params.Get<int>("altType"), _params.Get<int>("alt"));
	coord->SetAlt(alt);
}

}
