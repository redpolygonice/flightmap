#ifndef DOG_H
#define DOG_H

#include "common/types.h"
#include "common/singleton.h"
#include "devices/connectionmanager.h"
#include "devices/connection.h"
#include "common/devicetypes.h"
#include "data/srtm.h"

namespace core
{

// The main core class
class Dog : public Singleton<Dog>
{
	friend class Singleton<Dog>;

private:
	bool _connected;
	device::ConnectionManagerPtr _connections;
	data::Srtm _srtm;

private:
	Dog();

public:
	~Dog();

public:
	void stop();
	bool isConnected() const { return _connected; }
	bool connect(const common::ConnectParams &params);
	void disconnect();
	device::ConnectionPtr activeConnection() const { return _connections->active(); }
	data::TeleBoxPtr telebox() const { return activeConnection() == nullptr ? nullptr : activeConnection()->device()->telebox(); }
	float getAltitude(float lat, float lon) { return _srtm.getAltitude(lat, lon); }
};

}

#endif // DOG_H
