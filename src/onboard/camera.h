#ifndef CAMERA_H
#define CAMERA_H

#include "common/types.h"

namespace onboard
{

class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using ImageReadyFunction = std::function<void()>;

class Camera
{
private:
	uint32_t _imageSize;
	uint16_t _chunkCount;
	uint16_t _chunkRemainder;
	uint16_t _currentChunk;
	ByteArray _imgData;
	ByteArray _chunkData;
	ImageReadyFunction _imageReady;

	bool _isImageMagic;
	bool _isChunkMagic;
	const uint16_t kBlock = 4096;
	const uint8_t kImageMagic1 = 0xaf;
	const uint8_t kImageMagic2 = 0xca;
	const uint8_t kChunkMagic1 = 0xad;
	const uint8_t kChunkMagic2 = 0xbc;

public:
	Camera();
	~Camera();

public:
	static CameraPtr Create() { return std::make_shared<Camera>(); }
	void Init(uint16_t size = 0);
	void SetSize(uint16_t size) { _imageSize = size; }
	void ParseBytes(const ByteArray &buffer);
	void ImageReady(const ImageReadyFunction &func) { _imageReady = func; }

private:
	void SaveImage();
};

}

#endif // CAMERA_H
