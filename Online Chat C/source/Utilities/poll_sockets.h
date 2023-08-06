#pragma once
#include <stdio.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

static const int POLL_INITIAL_SIZE = 5;

struct PollSockets {
	int poll_count;
	int poll_size;
	WSAPOLLFD* polling_sockets;
};

void CreatePoll(struct PollSockets* poll_sockets);

void AddToPoll(struct PollSockets* poll_sockets, SOCKET new_socket);
void RemoveFromPoll(struct PollSockets* poll_sockets, int socket_iterator);

void RemoveAllSockets(struct PollSockets* poll_sockets);