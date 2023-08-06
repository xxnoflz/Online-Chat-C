#pragma once
#include <stdio.h>
#include "packet_serialization.h"

BOOL ChatSendPacket(SOCKET connect_socket, char* user_name, char* message);
BOOL ChatSendCompletePacket(SOCKET connect_socket, struct ChatPacket* packet_to_send);

void SendWholePacket(SOCKET connect_socket, char* packet, int packet_length);

struct ChatPacket* ChatReceivePacket(SOCKET connect_socket);