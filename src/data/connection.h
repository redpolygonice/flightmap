#ifndef CONNECTION_H
#define CONNECTION_H

#include "common/types.h"
#include "devices/idevice.h"
#include "comms/icommunication.h"

namespace data
{

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

class ConnectionList;
typedef std::shared_ptr<ConnectionList> ConnectionListPtr;
typedef std::vector<ConnectionPtr> ConnectionDataList;
typedef std::vector<ConnectionPtr>::iterator ConnectionListIterator;
typedef std::vector<ConnectionPtr>::const_iterator ConnectionListConstIterator;

// Connection object
class Connection
{
	friend class ConnectionList;

private:
	device::DevicePtr _device;
	int _number;
	bool _active;

public:
	Connection(const device::DevicePtr &device);
	~Connection();

public:
	static ConnectionPtr create(const device::DevicePtr &device) { return std::make_shared<Connection>(device); }
	void stop();
	device::DevicePtr device() const { return _device; }
	int number() const { return _number; }
	bool isActive() const { return _active; }
};

// Connection list object
class ConnectionList
{
private:
	ConnectionDataList _data;

public:
	ConnectionList() {}
	~ConnectionList() {}

public:
	static ConnectionListPtr create() { return std::make_shared<ConnectionList>(); }
	ConnectionPtr active() const;
	void setActive(int number);
	void add(const device::DevicePtr &device);
	void remove(const device::DevicePtr &device);
	void remove(int number);
	void removeActive();
	void removeAll();
	ConnectionPtr findByNumber(int number) const;
	device::DevicePtr findDeviceByNumber(int number) const;
};

}

#endif // CONNECTION_H
