#include "IPCServer.h"
#include "Logging.h"
#include "ServerTrackedDeviceProvider.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

void IPCServer::HandleRequest(const protocol::Request &request, protocol::Response &response)
{
	switch (request.type)
	{
	case protocol::RequestHandshake:
		response.type = protocol::ResponseHandshake;
		response.protocol.version = protocol::Version;
		break;

	case protocol::RequestSetDeviceTransform:
		driver->SetDeviceTransform(request.setDeviceTransform);
		response.type = protocol::ResponseSuccess;
		break;

	default:
		LOG("Invalid IPC request: %d", request.type);
		break;
	}
}

IPCServer::~IPCServer()
{
	Stop();
}

void IPCServer::Run()
{
	mainThread = std::thread(RunThread, this);
}

void IPCServer::Stop()
{
	if (!running)
		return;

	stop = true;
	if (listenFd >= 0)
		shutdown(listenFd, SHUT_RDWR);
	mainThread.join();
	running = false;
}

void IPCServer::RunThread(IPCServer *_this)
{
	_this->running = true;

	unlink(OPENVR_SPACECALIBRATOR_PIPE_NAME);

	_this->listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (_this->listenFd < 0)
	{
		LOG("Failed to create socket, error: %d", errno);
		return;
	}

	struct sockaddr_un addr = {};
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, OPENVR_SPACECALIBRATOR_PIPE_NAME, sizeof(addr.sun_path) - 1);

	if (bind(_this->listenFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		LOG("Failed to bind socket, error: %d", errno);
		close(_this->listenFd);
		return;
	}

	if (listen(_this->listenFd, 1) < 0)
	{
		LOG("Failed to listen on socket, error: %d", errno);
		close(_this->listenFd);
		return;
	}

	while (!_this->stop)
	{
		int clientFd = accept(_this->listenFd, nullptr, nullptr);
		if (clientFd < 0)
			break;

		LOG("IPC client connected");

		while (!_this->stop)
		{
			protocol::Request request;
			ssize_t bytesRead = read(clientFd, &request, sizeof request);
			if (bytesRead != sizeof request)
				break;

			protocol::Response response(protocol::ResponseInvalid);
			_this->HandleRequest(request, response);

			ssize_t bytesWritten = write(clientFd, &response, sizeof response);
			if (bytesWritten != sizeof response)
				break;
		}

		LOG("IPC client disconnected");
		close(clientFd);
	}

	close(_this->listenFd);
	unlink(OPENVR_SPACECALIBRATOR_PIPE_NAME);
}
