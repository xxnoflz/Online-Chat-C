#pragma once
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <WinSock2.h>

typedef uint16_t MetaData;

#define META_DATA_SIZE sizeof(MetaData)
#define MAX_USER_NAME_LENGTH 100
#define MAX_MESSAGE_LENGTH 256

struct ChatPacket {
	MetaData packet_size;	//Packet length in bytes counting the user name and message
	char user_name[MAX_USER_NAME_LENGTH];
	char message[MAX_MESSAGE_LENGTH];

	size_t user_name_length;
	size_t message_length;
};

char* SerializeData(struct ChatPacket* packet);
struct ChatPacket* DeserializeData(MetaData packet_size, char* packet);

struct ChatPacket* CreatePacket(char* user_name, char* message);