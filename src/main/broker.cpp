#include "broker.h"
#include "common/log.h"
#include "common/common.h"

#include "devices/idevice.h"
#include "comms/icommunication.h"
#include "comms/commbuilder.h"
#include "devices/pixhawk.h"
#include "devices/raspi.h"
#include "devices/mock.h"
#include "data/grid.h"

namespace core
{

#define CHECK_DEVICE()						\
	device::DevicePtr device = ActiveDevice();	\
	if (device == nullptr)						\
{											\
	LOGW("There is no connected device!");	\
	return false;							\
}

Broker::Broker()
	: _deviceManager(nullptr)
	, _timePoint(std::chrono::high_resolution_clock::now())
{
	_deviceManager = device::DeviceManager::Create();
	CreateImagePath();
}

Broker::~Broker()
{
}

void Broker::Stop()
{
	_deviceManager->RemoveAll();
}

bool Broker::IsConnected() const
{
	if (_deviceManager->Count() == 0)
		return false;

	return true;
}

bool Broker::Connect(const common::ConnectParams &params)
{
	// First create communication and check it
	comms::CommBuilder commBuilder;
	comms::CommunicationPtr comm = commBuilder.Create(params);
	if (comm == nullptr)
	{
		LOGE("Can't create communication!");
		return false;
	}

	// Create device
	device::DevicePtr device = nullptr;
	if (params.device == "Pixhawk")
	{
		device = device::Pixhawk::Create(comm);
	}
	else if (params.device == "Raspi")
	{
		device = device::Raspi::Create(comm);
	}
	else if (params.device == "Mock")
	{
		device = device::Mock::Create(comm);
	}
	else
	{
		LOGE("Can't create device, wrong params!");
		return false;
	}

	// Create connection object
	_deviceManager->Add(device);
	device->Start();

	LOG("Waiting heartbeat ..");
	if (!device->WaitHeartbeat())
	{
		LOGW("Device not working!");
		return false;
	}

	return true;
}

bool Broker::ConnectAsync(const common::ConnectParams &params, const ConnectFunction &func)
{
	std::thread([this,params,func]() {
		bool result = Connect(params);
		std::invoke(func, result);
	}).detach();
	return true;
}

void Broker::Disconnect()
{
	if (!IsConnected())
	{
		LOGW("There is no connected devices!");
		return;
	}

	device::DevicePtr device = ActiveDevice();
	if (device == nullptr)
		return;

	device->Stop();
	_deviceManager->Remove(device);

	LOG("Device " << device->Name() << " disconnected!");
}

void Broker::Disconnect(int number)
{
	if (!IsConnected())
	{
		LOGW("There is no connected devices!");
		return;
	}

	device::DevicePtr device =_deviceManager->FindDeviceByNumber(number);
	if (device == nullptr)
		return;

	device->Stop();
	_deviceManager->Remove(number);
	LOG("Device " << device->Name() << " disconnected!");
}

device::ConnectionPtr Broker::FindConnection(const common::ConnectParams &params) const
{
	return nullptr;
}

void Broker::SetAcive(int number)
{
	_deviceManager->SetActive(number);
}

float Broker::ComputeAlt(float lat, float lon, bool check, int type, int value)
{
	float alt = 50.0;
	common::AltType altType = static_cast<common::AltType>(type);

	if (check)
	{
		if (altType == common::AltType::Absolute || altType == common::AltType::Terrain)
			alt = value;
		else if (altType == common::AltType::Relative)
			alt = core::Broker::instance()->GetAltitude(lat, lon) + value;
	}

	return alt;
}

bool Broker::CreateMission(const data::CoordinateList &points, data::CoordinateList &mission, const common::AnyMap &params)
{
	data::Grid grid;
	if (!grid.Create(points, mission, params) || mission.empty())
	{
		LOG("Can't create mission points!");
		return false;
	}

	return true;
}

bool Broker::ClearMission()
{
	CHECK_DEVICE()
			device->ClearMission();
	return true;
}

bool Broker::WriteMission(const data::CoordinateList &points, const common::AnyMap &params)
{
	CHECK_DEVICE()
			device->WriteMission(points, params);
	return true;
}

bool Broker::WriteMissionAsync(const data::CoordinateList &points, const common::AnyMap &params)
{
	CHECK_DEVICE()
			std::thread([device,points,params]() {
		device->WriteMission(points, params);
	}).detach();
	return true;
}

bool Broker::ReadMission(data::CoordinateList &points)
{
	CHECK_DEVICE()
			device->ReadMission(points);
	return true;
}

bool Broker::ReadMissionAsync(const ReadMissionFunction &func)
{
	CHECK_DEVICE()
			std::thread([device,func]() {
		data::CoordinateList points;
		device->ReadMission(points);
		std::invoke(func, points);
	}).detach();
	return true;
}

bool Broker::SaveMissionToFile(const std::string &fileName, const data::CoordinateList &points, const common::AnyMap &params)
{
	data::Grid grid;
	if (!grid.SaveFile(fileName, points, params))
	{
		LOGE("Can't save mission!");
		return false;
	}

	return true;
}

bool Broker::LoadMissionFromFile(const std::string &fileName, data::CoordinateList &points, common::AnyMap &params)
{
	data::Grid grid;
	if (!grid.LoadFile(fileName, points, params))
	{
		LOGE("Can't load mission!");
		return false;
	}

	return true;
}

void Broker::ProcessKeyDown(KeyCode key)
{
	if (!PressControl())
		return;

	if (key == KeyCode::Left)
	{
		TurnLeft();
	}
	else if (key == KeyCode::Right)
	{
		TurnRight();
	}
	else if (key == KeyCode::Up)
	{
		MoveUp();
	}
	else if (key == KeyCode::Down)
	{
		MoveDown();
	}
	else if (key == KeyCode::W)
	{
		MoveForward();
	}
	else if (key == KeyCode::S)
	{
		MoveBack();
	}
	else if (key == KeyCode::A)
	{
		MoveLeft();
	}
	else if (key == KeyCode::D)
	{
		MoveRight();
	}
}

void Broker::ProcessKeyUp(KeyCode key)
{
	if (key == KeyCode::Left)
	{
		TurnLeft(true);
	}
	else if (key == KeyCode::Right)
	{
		TurnRight(true);
	}
	else if (key == KeyCode::Up)
	{
		MoveUp(true);
	}
	else if (key == KeyCode::Down)
	{
		MoveDown(true);
	}
	else if (key == KeyCode::W)
	{
		MoveForward(true);
	}
	else if (key == KeyCode::S)
	{
		MoveBack(true);
	}
	else if (key == KeyCode::A)
	{
		MoveLeft(true);
	}
	else if (key == KeyCode::D)
	{
		MoveRight(true);
	}
}

void Broker::StartMission()
{
	if (ActiveDevice() == nullptr)
		return;

	// Loiter
	Loiter();
	common::Sleep(1000);

	// Arm
	Arm(true);
	common::Sleep(2000);

	// Set channels
	DefaultChannels();

	ActiveDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
	common::Sleep(500);
}

void Broker::SetHome(const data::CoordinatePtr &point)
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SetHome(point->Lat(), point->Lon(), point->Alt());
}

void Broker::SetPosition(const data::CoordinatePtr &point)
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SetMode(device::FlightDevice::FlightModes::ModeGuided);
	ActiveDevice()->SetPosition(point->Lat(), point->Lon(), point->Alt());
}

void Broker::SetMode(uint32_t mode)
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SetMode(mode);
}

void Broker::Arm(bool arm)
{
	if (ActiveDevice() == nullptr)
		return;

	float param1 = (arm == true ? 1 : 0);
	float param2 = (arm == true ? common::kArmMagic : common::kDisarmMagic);

	ActiveDevice()->SendCommand(MAV_CMD_COMPONENT_ARM_DISARM, param1, param2);
}

void Broker::Loiter()
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SendCommand(MAV_CMD_NAV_LOITER_UNLIM, 1);
}

void Broker::rtl()
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SendCommand(MAV_CMD_NAV_RETURN_TO_LAUNCH, 1);
}

void Broker::Auto()
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SetMode(device::FlightDevice::FlightModes::ModeAuto);
}

void Broker::WriteChannel(int index, uint16_t rc)
{
	if (ActiveDevice() == nullptr)
		return;

	if (index < 1 || index > 4)
		return;

	//clearChannels();
	_channels[index] = rc;
	ActiveDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
}

void Broker::SetDefaultChannel(int index)
{
	if (ActiveDevice() == nullptr)
		return;

	if (index < 1 || index > 4)
		return;

	_channels[index] = defaultChannel;
	ActiveDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
}

void Broker::SetDefaultChannels()
{
	if (ActiveDevice() == nullptr)
		return;

	DefaultChannels();
	ActiveDevice()->RcChannelsOverride(_channels[1], _channels[2], _channels[3], _channels[4]);
}

void Broker::SendServoCommand(float index, float ch)
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SendCommand(MAV_CMD_DO_SET_SERVO, index, ch);
}

void Broker::SendCommand(uint32_t cmd, float param1, float param2, float param3, float param4, float param5, float param6, float param7)
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->SendCommand(cmd, param1, param2, param3, param4, param5, param6, param7);
}

void Broker::MoveUp(bool stop)
{
	if (stop)
		SetDefaultChannel(2);
	else
		WriteChannel(2, channelHigh);
}

void Broker::MoveDown(bool stop)
{
	if (stop)
		SetDefaultChannel(2);
	else
		WriteChannel(2, channelLow);
}

void Broker::MoveLeft(bool stop)
{
	if (stop)
		SetDefaultChannel(1);
	else
		WriteChannel(1, channelLow);
}

void Broker::MoveRight(bool stop)
{
	if (stop)
		SetDefaultChannel(1);
	else
		WriteChannel(1, channelHigh);
}

void Broker::MoveForward(bool stop)
{
	if (stop)
		SetDefaultChannel(3);
	else
		WriteChannel(3, channelHigh);
}

void Broker::MoveBack(bool stop)
{
	if (stop)
		SetDefaultChannel(3);
	else
		WriteChannel(3, channelLow);
}

void Broker::TurnLeft(bool stop)
{
	if (stop)
		SetDefaultChannel(4);
	else
		WriteChannel(4, channelLow);
}

void Broker::TurnRight(bool stop)
{
	if (stop)
		SetDefaultChannel(4);
	else
		WriteChannel(4, channelHigh);
}

void Broker::StartCamera()
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->StartCamera();
}

void Broker::StopCamera()
{
	if (ActiveDevice() == nullptr)
		return;

	ActiveDevice()->StopCamera();
}

bool Broker::PressControl()
{
	std::chrono::high_resolution_clock::time_point nowPoint = std::chrono::high_resolution_clock::now();
	int64_t count = std::chrono::duration_cast<std::chrono::milliseconds>(nowPoint - _timePoint).count();
	if (count < _timeInterval)
		return false;

	_timePoint = nowPoint;
	return true;
}

void Broker::CreateImagePath()
{
	string tmpDir;
#ifdef WIN32
	char *homeDir = getenv("USERPROFILE");
#else
	char *homeDir = getenv("HOME");
#endif
	if (homeDir == nullptr)
		tmpDir = "tmp";
	else
		tmpDir = string(homeDir) + PS + ".tmp";

	if (!common::IsFileExists(tmpDir))
		common::CreateDir(tmpDir);

	_imagePath =  tmpDir + PS + "camera.jpg";
}

}
