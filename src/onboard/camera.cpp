#include "camera.h"
#include "main/broker.h"

namespace onboard
{

Camera::Camera()
	: _imageSize(0)
	, _chunkCount(0)
	, _chunkRemainder(0)
	, _currentChunk(0)
	, _isImageMagic(false)
	, _isChunkMagic(false)
{
}

Camera::~Camera()
{
}

void Camera::Init(uint16_t size)
{
	_isImageMagic = false;
	_isChunkMagic = false;
	_imageSize = size;
	_chunkCount = 0;
	_chunkRemainder = 0;
	_currentChunk = 0;
	_imgData.clear();
	_chunkData.clear();
}

void Camera::ParseBytes(const ByteArray &buffer)
{
	int i = 0;
	while (i < buffer.size())
	{
		// Detect image magic number
		if (i + 1 < buffer.size() && _isImageMagic == false)
		{
			if (buffer[i] == kImageMagic1 && buffer[i + 1] == kImageMagic2)
			{
				_isImageMagic = true;
				i += 2;
			}
		}

		// Compute image size
		if (_isImageMagic == true && _imageSize == 0)
		{
			if (i + 1 < buffer.size())
			{
				memcpy(&_imageSize, &buffer[i], 2);
				_chunkCount = _imageSize / kBlock;
				_chunkRemainder = _imageSize % kBlock;
				i += 2;
			}
		}

		// Detect chunk magic number
		if (i + 1 < buffer.size() && _isChunkMagic == false)
		{
			if (buffer[i] == kChunkMagic1 && buffer[i + 1] == kChunkMagic2)
			{
				_isChunkMagic = true;
				i += 2;
			}
		}

		// Apend data
		if (i < buffer.size() && _isChunkMagic == true)
		{
			_chunkData.push_back(buffer[i]);

			uint16_t chunkSize = kBlock;
			if (_currentChunk >= _chunkCount)
				chunkSize = _chunkRemainder;

			if (chunkSize == 0)
			{
				++i;
				continue;
			}

			if (_chunkData.size() >= chunkSize)
			{
				std::copy(_chunkData.begin(), _chunkData.end(), std::back_inserter(_imgData));
				_isChunkMagic = false;
				_chunkData.clear();
				_currentChunk++;

				if (_imgData.size() >= _imageSize)
				{
					_imgData.resize(_imageSize);
					SaveImage();
					Init();
				}
			}
		}

		++i;
	}
}

void Camera::SaveImage()
{
	string fileName = core::Broker::instance()->ImageLocation();
	FILE *fp = fopen(fileName.c_str(), "wb");
	if (fp == nullptr)
		return;

	fwrite((void*)_imgData.data(), _imgData.size(), 1, fp);
	fclose(fp);
}

}
