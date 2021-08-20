#include "intercept.h"
#include "utils.h"
#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

int main() {
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionKeyStroke stroke;
	wchar_t hardware_id[500];
	WSADATA wsaData;
	WSAStartup(0x0202, &wsaData);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(55554);
	int clientSock = socket(PF_INET, SOCK_DGRAM, 0);
	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_E0);
	while (interception_receive(context, device = interception_wait(context), (InterceptionStroke*)&stroke, 1) > 0) {
		size_t deviceID = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));
		std::cout << deviceID;
		interception_send(context, device, (InterceptionStroke*)&stroke, 1);
		char devID[1000] = "";
		snprintf(devID, sizeof deviceID, "%zu", deviceID);
		sendto(clientSock, devID, sizeof(devID), 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
		break;
	}
	closesocket(clientSock);
	WSACleanup();
	interception_destroy_context(context);
	return 0;
}
