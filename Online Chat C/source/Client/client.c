#include "client.h"

void StartClient(void) {
	char user_name[MAX_USER_NAME_LENGTH] = "";
	InputUserName(user_name);

	SOCKET connected_socket;
	CreateClientSocket(&connected_socket);

	struct PollSockets poll_socket;
	CreatePoll(&poll_socket);
	AddToPoll(&poll_socket, connected_socket);

	RunClient(&poll_socket, &connected_socket, user_name);
}

void InputUserName(char* user_name) {
	printf("User name: ");
	while(gets_s(user_name, MAX_USER_NAME_LENGTH) == NULL);
}

void CreateClientSocket(SOCKET* connected_socket) {
	struct addrinfo hints;
	struct addrinfo* address_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char server_ip[INET6_ADDRSTRLEN] = "";

	printf("Server ip address: ");
	gets_s(server_ip, INET6_ADDRSTRLEN);

	while(getaddrinfo(server_ip, SERVER_PORT, &hints, &address_info) != 0) {
		printf("Connection failed! Try again\n");

		printf("Server ip address: ");
		gets_s(server_ip, INET6_ADDRSTRLEN);
	}

	struct addrinfo* address_iterator;
	for(address_iterator = address_info; address_iterator != NULL; address_iterator = address_iterator->ai_next) {

		*connected_socket = socket(address_iterator->ai_family, address_iterator->ai_socktype, address_iterator->ai_protocol);
		if(*connected_socket == INVALID_SOCKET) {
			printf("socket creation failed with: %d\n", WSAGetLastError());
			freeaddrinfo(address_info);
			WSACleanup();
			exit(-3);
		}

		if(connect(*connected_socket, address_iterator->ai_addr, (int)address_iterator->ai_addrlen) == SOCKET_ERROR) {
			closesocket(*connected_socket);

			continue;
		}

		break;
	}
	freeaddrinfo(address_info);

	if(address_iterator == NULL) {
		printf("failed to connect with: %d\n", WSAGetLastError());
		closesocket(*connected_socket);
		WSACleanup();
		exit(-4);
	}
}

void RunClient(struct PollSockets* poll_socket, SOCKET* connected_socket, char* user_name) {
	char message_input_buffer[MAX_MESSAGE_LENGTH] = "";

	while(TRUE) {
		printf("> ");
		gets_s(message_input_buffer, MAX_MESSAGE_LENGTH);

		if(ChatSendPacket(*connected_socket, user_name, message_input_buffer) == FALSE)
			printf("send fail!\n");

		int number_of_events = WSAPoll(poll_socket->polling_sockets, poll_socket->poll_count, 0);
		if(number_of_events == SOCKET_ERROR) {
			printf("poll error: %d\n", WSAGetLastError());
			RemoveAllSockets(poll_socket);
			WSACleanup();
			exit(-7);
		}

		if(poll_socket->polling_sockets[0].revents & POLLIN) { //Input event
			struct ChatPacket* received_packet = ChatReceivePacket(*connected_socket);
			if(received_packet == NULL)
				continue;

			printf("%s: %s\n", received_packet->user_name, received_packet->message);

			free(received_packet);
		}

		if(poll_socket->polling_sockets[0].revents & (POLLHUP | POLLERR)) { //Connection closed or error event
			printf("connection closed\n");
			RemoveAllSockets(poll_socket);
			WSACleanup();
			exit(-8);
		}

	}
}