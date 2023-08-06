#include "server.h"

void StartServer(void) {
	SOCKET listener_socket;
	CreateServerSocket(&listener_socket);

	struct PollSockets poll_sockets;
	CreatePoll(&poll_sockets);
	AddToPoll(&poll_sockets, listener_socket);

	RunServer(&poll_sockets, listener_socket);
}

void CreateServerSocket(SOCKET* listener_socket) {
	BOOL socket_reuse = TRUE;

	struct addrinfo hints;
	struct addrinfo* address_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if(getaddrinfo(NULL, LISTENING_PORT, &hints, &address_info) != 0) {
		printf("getaddrinfo failed with: %d\n", WSAGetLastError());
		WSACleanup();
		exit(-2);
	}

	struct addrinfo* address_iterator;
	for(address_iterator = address_info; address_iterator != NULL; address_iterator = address_iterator->ai_next) {
		*listener_socket = socket(address_iterator->ai_family, address_iterator->ai_socktype, address_iterator->ai_protocol);

		if(*listener_socket == INVALID_SOCKET) {
			printf("socket creation failed with: %d\n", WSAGetLastError());
			freeaddrinfo(address_info);
			WSACleanup();
			exit(-3);
		}
		
		if(setsockopt(*listener_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&socket_reuse, sizeof(BOOL)) == SOCKET_ERROR) {
			printf("setsockopt failed with: %d\n", WSAGetLastError());
			closesocket(*listener_socket);
			freeaddrinfo(address_info);
			WSACleanup();
			exit(-3);
		}

		if(bind(*listener_socket, address_iterator->ai_addr, (int)address_iterator->ai_addrlen) == SOCKET_ERROR) {
			closesocket(*listener_socket);

			continue;
		}

		break;
	}

	freeaddrinfo(address_info);

	if(address_iterator == NULL) {
		printf("failed to bind socket with: %d\n", WSAGetLastError());
		closesocket(*listener_socket);
		WSACleanup();
		exit(-4);
	}

	if(listen(*listener_socket, BACKLOG) == SOCKET_ERROR) {
		printf("listen failed with: %d\n", WSAGetLastError());
		closesocket(*listener_socket);
		WSACleanup();
		exit(-5);
	}
}

void RunServer(struct PollSockets* poll_sockets, SOCKET listener_socket) {
	char new_user_address_string[INET6_ADDRSTRLEN] = "";

	while(TRUE) {
		int number_of_events = WSAPoll(poll_sockets->polling_sockets, poll_sockets->poll_count, -1);

		if(number_of_events == SOCKET_ERROR) {
			printf("poll error: %d\n", WSAGetLastError());
			RemoveAllSockets(poll_sockets);
			WSACleanup();
			exit(-7);
		}

		for(int socket_iterator = 0; socket_iterator < poll_sockets->poll_count; ++socket_iterator) {

			if(poll_sockets->polling_sockets[socket_iterator].revents & POLLIN) { //Input event

				if(poll_sockets->polling_sockets[socket_iterator].fd == listener_socket) { //Listening socket
					struct sockaddr_storage client_address;
					socklen_t client_address_length = sizeof(client_address);
					SOCKET new_socket = accept(listener_socket, (struct sockaddr*)&client_address, &client_address_length);

					if(new_socket == INVALID_SOCKET)
						continue;

					AddToPoll(poll_sockets, new_socket);

					inet_ntop(client_address.ss_family, GetClientAddress((struct sockaddr*)&client_address), new_user_address_string, sizeof(new_user_address_string));
					printf("New connection from: %s\n", new_user_address_string);
				}
				else { //Client socket
					SOCKET sender_socket = poll_sockets->polling_sockets[socket_iterator].fd;

					struct ChatPacket* received_packet = ChatReceivePacket(sender_socket);
					if(received_packet == NULL)
						continue;

					printf("Received: %s -- from %s\n", received_packet->message, received_packet->user_name);
					for(int second_socket_iterator = 0; second_socket_iterator < poll_sockets->poll_count; ++second_socket_iterator) {
						SOCKET destination_socket = poll_sockets->polling_sockets[second_socket_iterator].fd;

						if(destination_socket != listener_socket && destination_socket != sender_socket)
							if(ChatSendCompletePacket(destination_socket, received_packet) == FALSE)
								printf("send fail!\n");
					}

					free(received_packet);
				}

			}

			if(poll_sockets->polling_sockets[socket_iterator].revents & (POLLHUP | POLLERR)) { //Connection closed or error event
				closesocket(poll_sockets->polling_sockets[socket_iterator].fd);
				RemoveFromPoll(poll_sockets, socket_iterator);
			}
		}
	}
}

void* GetClientAddress(struct sockaddr* socket_address) {
	if(socket_address->sa_family == AF_INET)
		return &(((struct sockaddr_in*)socket_address)->sin_addr);
	return &(((struct sockaddr_in6*)socket_address)->sin6_addr);
}