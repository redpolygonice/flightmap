#include "camera.h"
#include "main/broker.h"
#include <cstdint>
#include <cstring>

namespace onboard
{

Camera::Camera()
	: _imageSize(0)
	, _isMagic(false)
{
}

Camera::~Camera()
{
	Init();
}

void Camera::Init()
{
	_imageSize = 0;
	_isMagic = false;
	_data.clear();
}

void Camera::ParseBytes(const ByteArray &buffer)
{
	// Check header
	if (buffer.size() < buffer.size())
		return;

	for (int i = 0; i < buffer.size(); ++i)
	{
		// Detect magic number
		if (i + 1 < buffer.size() && _isMagic == false)
		{
			if (buffer[i] == magic1 && buffer[i + 1] == magic2)
			{
				_isMagic = true;
				++i;
				continue;
			}
		}

		// Got start position - compute size
		if (_isMagic == true && _imageSize == 0)
		{
			if (i + 1 < buffer.size())
			{
				std::memcpy(&_imageSize, &buffer[i], 2);
				_data.reserve(_imageSize);
				++i;
			}
		}
		else if (_isMagic == true && _imageSize > 0)
		{
			_data.push_back(buffer[i]);
			if (_data.size() >= _imageSize)
			{
				SaveImage();
				if (_imageReady != nullptr)
					_imageReady();
				Init();
			}
		}
	}
}

void Camera::SaveImage()
{
	string fileName = core::Broker::instance()->ImageLocation();
	FILE *fp = fopen(fileName.c_str(), "wb");
	if (fp == nullptr)
		return;

	fwrite((void*)_data.data(), _data.size(), 1, fp);
	fclose(fp);
}

}
