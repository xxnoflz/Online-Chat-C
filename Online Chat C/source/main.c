#include <stdio.h>
#include <string.h>

#include "Utilities/wsa_init.h"
#include "Server/server.h"
#include "Client/client.h"

void StartChat(char* chat_type) {
	if(strcmp("-server", chat_type) == 0)
		StartServer();
	else if(strcmp("-client", chat_type) == 0)
		StartClient();
	else {
		printf("Incorrect chat type!\n");

		WSACleanup();
		exit(-1);
	}
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Provide chat type!\n");

		return -1;
	}
	
	WSADATA wsa_data;
	WSA_Initialize(&wsa_data);
	
	StartChat(argv[1]);

	WSACleanup();
	return 0;
}