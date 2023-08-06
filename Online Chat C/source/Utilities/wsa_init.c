#include "wsa_init.h"

void WSA_Initialize(WSADATA* wsa_data) {
	if(WSAStartup(MAKEWORD(2, 2), wsa_data) != 0) 
		exit(1); //WSAStartup failed
	
	if(LOBYTE(wsa_data->wVersion) != 2 || HIBYTE(wsa_data->wVersion) != 2) {
		WSACleanup();
		exit(2); //Winsock 2.2 is not supported
	}
}