#include "IPCClient.h"

#include <sys/socket.h>
#include <string>
#include <sys/un.h>
#include <format>
#include <stdexcept>
#include <unistd.h>

#define CONN_ERROR_MSG "Space Calibrator driver unavailable. Make sure SteamVR is running, and the Space Calibrator addon is enabled in SteamVR settings."
#define DRIVER_VERSION_ERROR_MSG \
"Incorrect driver version installed, try reinstalling OpenVR-SpaceCalibrator. (Client: ({}), Driver: ({}))"

IPCClient::~IPCClient() {
    if (client_fd >= 0) close(client_fd);
}

protocol::Response IPCClient::SendBlocking(const protocol::Request &request) {
    Send(request);
    return Receive();
}

static void Handshake(IPCClient &client, int conn) {
    if (conn == -1) throw std::runtime_error(CONN_ERROR_MSG);

    auto payload = protocol::Request(protocol::RequestHandshake);
    auto response = client.SendBlocking(payload);

    if (response.type != protocol::ResponseHandshake || response.protocol.version != protocol::Version) {
        std::string errorMessage = std::format(DRIVER_VERSION_ERROR_MSG, std::to_string(protocol::Version),
                                               std::to_string(response.protocol.version));
        throw std::runtime_error(errorMessage);
    }
}

void IPCClient::Connect() {
    this->client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,OPENVR_SPACECALIBRATOR_PIPE_NAME, sizeof(addr.sun_path) - 1);

    int conn = connect(client_fd, (sockaddr *) &addr, sizeof(addr));

    Handshake(*this,conn);
}


protocol::Response IPCClient::Receive() {
    protocol::Response response{};
    ssize_t response_size = recv(client_fd, &response, sizeof(response), 0);

    if (response_size <= 0) {
        throw std::runtime_error("Error reading IPC response");
    }

    return response;
}
