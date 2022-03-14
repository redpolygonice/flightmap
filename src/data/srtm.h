#ifndef SRTM_H
#define SRTM_H

#include "common/types.h"

namespace data
{

// Shuttle Radar Topography Mission - allows detect altitude
class Srtm
{
private:
	typedef std::vector<std::vector<short>> ShortArray2d;
	std::map<string, ShortArray2d> _cache;
	std::map<int, string> _filenameMap;
	std::vector<string> _oceanNames;
	string _srtmPath;
	string _dataPath;

private:
	string getFilename(float lat, float lon);
	bool copyFile(const string &fileName) const;
	float getAlt(string filename, int x, int y);
	float avg(float v1, float v2, float weight);

public:
	Srtm();
	float getAltitude(float lat, float lon, float zoom = 16);
};

}

#endif // SRTM_H
