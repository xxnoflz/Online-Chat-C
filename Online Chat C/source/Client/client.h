#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#include "../Utilities/poll_sockets.h"
#include "../Utilities/common_networking.h"

static const char SERVER_PORT[] = "3490";

void StartClient(void);

void InputUserName(char* user_name);

void CreateClientSocket(SOCKET* connected_socket);

void RunClient(struct PollSockets* poll_socket, SOCKET* connected_socket, char* user_name);