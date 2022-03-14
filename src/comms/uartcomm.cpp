#include "uartcomm.h"
#include "common/log.h"
#include "main/dog.h"

#ifndef WIN32
#include <termios.h>
#endif

namespace comms
{

UartComm::UartComm(const common::ConnectParams &params)
	: ICommunication(params)
	, _active(false)
	, _handle(0)
	, _callback(nullptr)
{
}

UartComm::~UartComm()
{
}

bool UartComm::open()
{
	string dev = _connectParams.protocol;
	setBaudRate(_connectParams.baudrate);

#ifdef WIN32
	string fileName = dev; // "\\\\.\\" + dev;
	_handle = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
						 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (_handle == INVALID_HANDLE_VALUE)
	{
		LOGE("[UartComm] Can't open device " << dev << ", error: " << GetLastError());
		return false;
	}

	if (!PurgeComm(_handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
	{
		LOGE("[UartComm] PurgeComm error: " << GetLastError());
		return false;
	}

	if (!SetupComm(_handle, 4096, 4096))
	{
		LOGE("[UartComm] SetupComm error: " << GetLastError());
		return false;
	}

	if (!SetCommMask(_handle, EV_RXCHAR))
	{
		LOGE("[UartComm] SetCommMask error: " << GetLastError());
		return false;
	}

	// Set timeouts
	COMMTIMEOUTS timeouts;
	memset(&timeouts, 0, sizeof(timeouts));
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 500;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 500;

	if (!SetCommTimeouts(_handle, &timeouts))
	{
		LOGE("[UartComm] SetCommTimeouts error: " << GetLastError());
		return false;
	}

	// Set serial params
	DCB dcb;
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);

	if (!GetCommState(_handle, &dcb))
	{
		LOGE("[UartComm] GetCommState error: " << GetLastError());
		return false;
	}

	dcb.BaudRate = _baudrate;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.fBinary = 1;
	dcb.XonLim = 4096;
	dcb.XoffLim = 4096;

	if (!SetCommState(_handle, &dcb))
	{
		LOGE("[UartComm] SetCommState error: " << GetLastError());
		return false;
	}

	return true;
#else
	// sudo chmod a+rw /dev/ttyACM0
	// sudo usermod -a -G dialout `whoami`
	// sudo usermod -a -G uucp `whoami`

	string device = "/dev/" + dev;
	_handle = ::open(device.c_str(), O_RDWR | O_NOCTTY);
	if (_handle < 0)
	{
		LOGE("[UartComm] Can't open device: " << device);
		return false;
	}

	struct termios tty;
	memset(&tty, 0, sizeof(tty));

	if (tcgetattr(_handle, &tty) != 0)
	{
		LOG("[UartComm] tcgetattr error");
		return false;
	}

	cfsetospeed(&tty, _baudrate);
	cfsetispeed(&tty, _baudrate);

	tty.c_cflag &= ~PARENB;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag &= ~CSIZE;

	tty.c_cflag |= CS8 | CLOCAL | CREAD;
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_cc[VMIN] = 0;
	tty.c_cc[VTIME] = 10;

	cfmakeraw(&tty);
	tcflush(_handle, TCIFLUSH);
	if (tcsetattr(_handle, TCSANOW, &tty) != 0)
	{
		LOGE("[UartComm] tcsetattr error");
		return false;
	}

	return true;
#endif
}

void UartComm::close()
{
	_active = false;
	if (_thread.joinable())
		_thread.join();

#ifdef WIN32
	CloseHandle(_handle);
#else
	::close(_handle);
#endif
}

void UartComm::start(const ReadFunction &callback)
{
	_callback = callback;
	_active = true;
	_thread = std::thread([this]() { run(); });
}

bool UartComm::write(const unsigned char *buffer, size_t size)
{
#ifdef WIN32
	DWORD dwBytesWritten = 0;
	WriteFile(_handle, buffer, size, &dwBytesWritten, nullptr);
	if (dwBytesWritten > 0)
		return true;
	return false;
#else
	if (::write(_handle, buffer, size) > 0)
		return true;

	return false;
#endif
}

void UartComm::run()
{
#ifdef WIN32
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	DWORD dwBytesRed = 0;

	while (_active)
	{
		ReadFile(_handle, buffer, BUFFER_SIZE, &dwBytesRed, nullptr);
		if (dwBytesRed > 0)
		{
			if (_callback != nullptr)
			{
				ByteArray data(buffer, buffer + dwBytesRed);
				_callback(data);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
#else
	unsigned char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);

	while (_active)
	{
		ssize_t size = ::read(_handle, buffer, BUFFER_SIZE);
		if (size > 0)
		{
			if (_callback != nullptr)
			{
				ByteArray data(buffer, buffer + size);
				_callback(data);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	_active = false;
#endif
}

#ifndef WIN32
void UartComm::setBaudRate(uint32_t rate)
{
	_baudrate = B57600;
	switch (rate)
	{
	case 57600:
		_baudrate = B57600;
		break;
	case 115200:
		_baudrate = B115200;
		break;
	case 230400:
		_baudrate = B230400;
		break;
	case 460800:
		_baudrate = B460800;
		break;
	case 500000:
		_baudrate = B500000;
		break;
	case 576000:
		_baudrate = B576000;
		break;
	case 921600:
		_baudrate = B921600;
		break;
	}
}
#endif

}
