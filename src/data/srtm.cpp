#include "srtm.h"
#include "common/common.h"

#include <math.h>

namespace data
{

Srtm::Srtm()
{
#ifdef WIN32
	_srtmPath = "d:\\distr\\srtm";
#else
	_srtmPath = "/home/alexey/distr/srtm";
	if (!common::IsFileExists(_srtmPath))
		_srtmPath = "/home/aleksey/distr/srtm";
#endif
	_dataPath = common::GetCurrentDir() + PS + "srtm";

	if (!common::IsFileExists(_dataPath))
		common::CreateDir(_dataPath);

	// Create filename list
	for (int y = -90; y <= 90; y++)
	{
		char sy[3];
		sprintf(sy, "%0*d", 2, abs(y));

		for (int x = -180; x <= 180; x++)
		{
			char sx[4];
			sprintf(sx, "%0*d", 3, abs(x));

			char szFileName[255];
			sprintf(szFileName, "%s%s%s%s%s", y >= 0 ? "N" : "S", sy, x >= 0 ? "E" : "W", sx, ".hgt");
			_filenameMap[y * 1000 + x] = szFileName;
		}
	}

	// Create ocean filename list
	_oceanNames = { "00W000", "00W001", "01W000", "01W001", "00E000", "00E001", "01E000", "01E001" };
}

std::string Srtm::GetFileName(float lat, float lon)
{
	int x = (int)floor(lon);
	int y = (int)floor(lat);
	int id = y * 1000 + x;

	if (_filenameMap.find(id) != _filenameMap.end())
		return _filenameMap[id];

	return string();
}

bool Srtm::CopyFile(const std::string &fileName) const
{
	string srtmFile = _srtmPath + PS + fileName + ".zip";
	if (!common::IsFileExists(srtmFile))
		return false;

	if (!common::Unzip(srtmFile, _dataPath))
		return false;

	return true;
}

float Srtm::GetAlt(std::string filename, int x, int y)
{
	return _cache[filename][x][y];
}

float Srtm::Avg(float v1, float v2, float weight)
{
	return v2 * weight + v1 * (1 - weight);
}

float Srtm::GetAltitude(float lat, float lon, float zoom)
{
	string fileName = GetFileName(lat, lon);
	if (fileName.empty())
		return 0.0f;

	for (const string &name : _oceanNames)
	{
		if (fileName.find(name) != string::npos)
			return 0.0f;
	}

	string filePath = _dataPath + PS + fileName;
	if (!common::IsFileExists(filePath))
	{
		if (!CopyFile(fileName))
			return 0.0f;
	}

	if (_cache.find(fileName) != _cache.end() || common::IsFileExists(filePath))
	{
		int size = -1;

		// There is no in cash - add it
		if (_cache.find(fileName) == _cache.end())
		{
			std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
			if (!ifs.is_open())
				return 0.0f;

			ifs.seekg(0, std::ios::end);
			int length = ifs.tellg();
			ifs.seekg(0, std::ios::beg);

			if (length == (1201 * 1201 * 2))
				size = 1201;
			else if (length == (3601 * 3601 * 2))
				size = 3601;
			else
				return 0.0f;

			uint8_t altbytes[2];
			ShortArray2d altdata(size);
			for (int i = 0; i < altdata.size(); ++i)
				altdata[i].resize(size);

			int altlat = 0;
			int altlng = 0;

			while (!ifs.eof())
			{
				ifs.read((char *)altbytes, 2);
				altdata[altlat][altlng] = (short)((altbytes[0] << 8) + altbytes[1]);

				altlat++;
				if (altlat >= size)
				{
					altlng++;
					altlat = 0;
				}
			}

			_cache[fileName] = altdata;
		}

		int cacheLength = _cache[fileName].size() * _cache[fileName].size();
		if (cacheLength == (1201 * 1201))
			size = 1201;
		else if (cacheLength == (3601 * 3601))
			size = 3601;
		else
			return 0.0f;

		int x = (int)floor(lon);
		int y = (int)floor(lat);

		// remove the base lat long
		lat -= y;
		lon -= x;

		// values should be 0-1199, 1200 is for interpolation
		float xf = lon * (size - 1);
		float yf = lat * (size - 1);

		int x_int = (int)xf;
		float x_frac = xf - x_int;

		int y_int = (int)yf;
		float y_frac = yf - y_int;

		y_int = (size - 2) - y_int;

		float alt00 = GetAlt(fileName, x_int, y_int);
		float alt10 = GetAlt(fileName, x_int + 1, y_int);
		float alt01 = GetAlt(fileName, x_int, y_int + 1);
		float alt11 = GetAlt(fileName, x_int + 1, y_int + 1);

		float v1 = Avg(alt00, alt10, x_frac);
		float v2 = Avg(alt01, alt11, x_frac);
		float v = Avg(v1, v2, 1 - y_frac);

		if (v < -1000)
			return 0.0f;

		return v;
	}

	return 0.0f;
}

}
