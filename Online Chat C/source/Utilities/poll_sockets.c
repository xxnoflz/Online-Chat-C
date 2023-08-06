#include "poll_sockets.h"

void CreatePoll(struct PollSockets* poll_sockets) {
	poll_sockets->poll_count = 0;
	poll_sockets->poll_size = POLL_INITIAL_SIZE;
	poll_sockets->polling_sockets = (WSAPOLLFD*)malloc(sizeof(*poll_sockets->polling_sockets) * poll_sockets->poll_size);

	if(poll_sockets->polling_sockets == NULL) {
		printf("failed to create poll socket\n");
		WSACleanup();
		exit(-6);
	}
}

void AddToPoll(struct PollSockets* poll_sockets, SOCKET new_socket) {
	if(poll_sockets->poll_count == poll_sockets->poll_size) {
		poll_sockets->poll_size *= 2;

		WSAPOLLFD* temporary_realloc = (WSAPOLLFD*)realloc(poll_sockets->polling_sockets, sizeof(*poll_sockets->polling_sockets) * poll_sockets->poll_size);
		if(temporary_realloc == NULL) {
			printf("failed to add socket to poll\n");
			WSACleanup();
			exit(-8);
		}
		poll_sockets->polling_sockets = temporary_realloc;
	}

	poll_sockets->polling_sockets[poll_sockets->poll_count].fd = new_socket;
	poll_sockets->polling_sockets[poll_sockets->poll_count].events = POLLIN;
	poll_sockets->polling_sockets[poll_sockets->poll_count].revents = 0;

	++poll_sockets->poll_count;
}

void RemoveFromPoll(struct PollSockets* poll_sockets, int socket_iterator) {
	poll_sockets->polling_sockets[socket_iterator] = poll_sockets->polling_sockets[poll_sockets->poll_count - 1];

	--poll_sockets->poll_count;
}

void RemoveAllSockets(struct PollSockets* poll_sockets) {
	for(int socket_iterator = 0; socket_iterator < poll_sockets->poll_count; ++socket_iterator)
		closesocket(poll_sockets->polling_sockets[socket_iterator].fd);
}