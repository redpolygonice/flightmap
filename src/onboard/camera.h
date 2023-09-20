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
	uint16_t _imageSize;
	ByteArray _data;
	ImageReadyFunction _imageReady;

	bool _isMagic;
	const uint8_t magic1 = 0xff;
	const uint8_t magic2 = 0xff;
	const uint16_t headerSize = 4;

public:
	Camera();
	~Camera();

public:
	static CameraPtr Create() { return std::make_shared<Camera>(); }
	void Init();
	void ParseBytes(const ByteArray &buffer);
	void ImageReady(const ImageReadyFunction &func) { _imageReady = func; }

private:
	void SaveImage();
};

}

#endif // CAMERA_H
