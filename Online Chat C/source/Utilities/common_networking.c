#include "common_networking.h"

BOOL ChatSendPacket(SOCKET connect_socket, char* user_name, char* message) {
	struct ChatPacket* packet_to_send = CreatePacket(user_name, message);
	if(packet_to_send == NULL)
		return FALSE;

	char* send_buffer = SerializeData(packet_to_send);
	if(send_buffer == NULL)
		return FALSE;

	SendWholePacket(connect_socket, send_buffer, META_DATA_SIZE + packet_to_send->packet_size);

	free(send_buffer);
	free(packet_to_send);

	return TRUE;
}

BOOL ChatSendCompletePacket(SOCKET connect_socket, struct ChatPacket* packet_to_send) {
	char* send_buffer = SerializeData(packet_to_send);
	if(send_buffer == NULL)
		return FALSE;

	SendWholePacket(connect_socket, send_buffer, META_DATA_SIZE + packet_to_send->packet_size);

	free(send_buffer);

	return TRUE;
}

void SendWholePacket(SOCKET connect_socket, char* packet, int packet_length) {
	int total_sent = 0;
	int bytes_left = packet_length;

	while(total_sent < packet_length) {
		int current_sent_bytes = send(connect_socket, packet + total_sent, bytes_left, 0);
		if(current_sent_bytes == SOCKET_ERROR) {
			printf("send error occured: %d\n", WSAGetLastError());
			closesocket(connect_socket);
			WSACleanup();
			exit(-9);
		}

		total_sent += current_sent_bytes;
		bytes_left -= current_sent_bytes;
	}
}

struct ChatPacket* ChatReceivePacket(SOCKET connect_socket) {
	MetaData packet_meta_data = 0;
	
	int meta_data_received = recv(connect_socket, (char*)&packet_meta_data, sizeof(packet_meta_data), 0);
	if(meta_data_received == SOCKET_ERROR) {
		printf("receive error occured: %d\n", WSAGetLastError());
		closesocket(connect_socket);
		WSACleanup();
		exit(-10);
	}

	MetaData packet_size = ntohs(packet_meta_data);

	char receive_buffer[MAX_USER_NAME_LENGTH + MAX_MESSAGE_LENGTH] = "";

	int receive_packet_bytes = recv(connect_socket, receive_buffer, packet_size, 0);
	if(receive_packet_bytes == SOCKET_ERROR) {
		printf("receive error occured: %d\n", WSAGetLastError());
		closesocket(connect_socket);
		WSACleanup();
		exit(-10);
	}

	struct ChatPacket* received_packet = DeserializeData(packet_size, receive_buffer);

	return received_packet;
}