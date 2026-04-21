#pragma once

#include "../Protocol.h"

#include <thread>

class ServerTrackedDeviceProvider;

class IPCServer
{
public:
	IPCServer(ServerTrackedDeviceProvider *driver) : driver(driver) { }
	~IPCServer();

	void Run();
	void Stop();

private:
	void HandleRequest(const protocol::Request &request, protocol::Response &response);
	static void RunThread(IPCServer *_this);

	std::thread mainThread;
	bool running = false;
	bool stop = false;
	int listenFd = -1;

	ServerTrackedDeviceProvider *driver;
};
