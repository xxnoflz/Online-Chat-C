#include "packet_serialization.h"

char* SerializeData(struct ChatPacket* packet) {
	char* serialized_data = (char*)malloc(META_DATA_SIZE + packet->packet_size);
	if(serialized_data == NULL)
		return NULL;

	char* serialized_packet_size_pointer = serialized_data;
	char* serialized_user_name_pointer = serialized_packet_size_pointer + sizeof(packet->packet_size);
	char* serialized_message_pointer = serialized_user_name_pointer + sizeof(packet->user_name);

	MetaData packet_size_network = htons(packet->packet_size);

	*((MetaData*)serialized_packet_size_pointer) = packet_size_network;
	strcpy_s(serialized_user_name_pointer, MAX_USER_NAME_LENGTH, packet->user_name);
	strcpy_s(serialized_message_pointer, sizeof(*packet->message) * packet->message_length, packet->message);

	return serialized_data;
}

struct ChatPacket* DeserializeData(MetaData packet_size, char* packet) {
	struct ChatPacket* deserialized_data = (struct ChatPacket*)malloc(sizeof(struct ChatPacket));
	if(deserialized_data == NULL)
		return NULL;

	memset(deserialized_data, 0, sizeof(struct ChatPacket));

	char* serialized_user_name_pointer = (char*)packet;
	char* serialized_message_pointer = (char*)(serialized_user_name_pointer + sizeof(deserialized_data->user_name));

	deserialized_data->packet_size = packet_size;
	strcpy_s(deserialized_data->user_name, MAX_USER_NAME_LENGTH, serialized_user_name_pointer);
	strcpy_s(deserialized_data->message, MAX_MESSAGE_LENGTH, serialized_message_pointer);

	deserialized_data->user_name_length = strnlen_s(deserialized_data->user_name, MAX_USER_NAME_LENGTH) + 1;	//With null terminator
	deserialized_data->message_length = strnlen_s(deserialized_data->message, MAX_MESSAGE_LENGTH) + 1;			//With null terminator

	return deserialized_data;
}

struct ChatPacket* CreatePacket(char* user_name, char* message) {
	struct ChatPacket* new_packet = (struct ChatPacket*)malloc(sizeof(struct ChatPacket));
	if(new_packet == NULL)
		return NULL;

	memset(new_packet, 0, sizeof(struct ChatPacket));

	new_packet->user_name_length = strnlen_s(user_name, MAX_USER_NAME_LENGTH) + 1;	//With null terminator
	new_packet->message_length = strnlen_s(message, MAX_MESSAGE_LENGTH) + 1;		//With null terminator

	new_packet->packet_size = (MetaData)(sizeof(new_packet->user_name) + (sizeof(*new_packet->message) * new_packet->message_length));
	strcpy_s(new_packet->user_name, MAX_USER_NAME_LENGTH, user_name);
	strcpy_s(new_packet->message, MAX_MESSAGE_LENGTH, message);

	return new_packet;
}