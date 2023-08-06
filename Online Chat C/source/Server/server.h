#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#include "../Utilities/poll_sockets.h"
#include "../Utilities/common_networking.h"

static const char LISTENING_PORT[] = "3490";
static const int BACKLOG = 10;

void StartServer(void);

void CreateServerSocket(SOCKET* listener_socket);

void RunServer(struct PollSockets* poll_sockets, SOCKET listener_socket);

void* GetClientAddress(struct sockaddr* socket_address);