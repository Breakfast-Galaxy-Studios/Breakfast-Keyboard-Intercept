#include <intercept.h>
#include <utils.h>
#include <iostream>
#include <main.h>
#include <thread>
#include <WinSock2.h>
#include <Windows.h>
#include <future>
#include <Ws2tcpip.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#pragma comment(lib, "Ws2_32.lib")
using Poco::AutoPtr;
using Poco::Util::PropertyFileConfiguration;
enum ScanCode {
	esc = 1,
	numlock = 69
};

bool operator==(const InterceptionKeyStroke& first,
	const InterceptionKeyStroke& second) {
	return first.code == second.code && first.state == second.state;
}

bool operator!=(const InterceptionKeyStroke& first,
	const InterceptionKeyStroke& second) {
	return first.code != second.code || first.state != second.state;
}

int getDevID() {
	std::string path = getenv("APPDATA");
	path += "\\BGS-Soundboard\\interception.properties";
	AutoPtr<PropertyFileConfiguration> config = new PropertyFileConfiguration(path);
	return config->getInt("devID");
}

bool closeProgram(std::string localhost) {
	struct sockaddr_in recieveAddr;
	recieveAddr.sin_family = AF_INET;
	InetPton(AF_INET, localhost.c_str(), &recieveAddr.sin_addr.s_addr);
	recieveAddr.sin_port = htons(55556);
	int serverAddrSize = sizeof(recieveAddr);
	SOCKET socketfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(socketfd, (struct sockaddr*)&recieveAddr, sizeof(recieveAddr));
	listen(socketfd, 20);
	accept(socketfd, NULL, NULL);
	exit(100);
}

int test(int ID) {
	std::string localhost = "127.0.0.1";
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionKeyStroke stroke, lastStroke;
	short int timesPressed = 0;
	wchar_t hardware_id[500];
	// E0 form of lshift, certain keys use this
	InterceptionKeyStroke lshiftE0 = { 42, INTERCEPTION_KEY_E0 };

	
	// Socket Stuff
	WSADATA wsaData;
	int server_port = 55555;
	WSAStartup(0x0202, &wsaData);
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, localhost.c_str(), &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons((u_short)server_port);
	int clientSock = socket(PF_INET, SOCK_DGRAM, 0);

	raise_process_priority();
	context = interception_create_context();
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_E0 | INTERCEPTION_FILTER_KEY_E1);
	auto f = std::async(std::launch::async, closeProgram, localhost);
	while (interception_receive(context, device = interception_wait(context), (InterceptionStroke*)&stroke, 1) > 0) {
		size_t deviceID = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));
		const int secondDevice = ID;
		if (deviceID == secondDevice) {
			if (stroke.code == numlock) {
				interception_send(context, device, (InterceptionStroke*)&stroke, 1);
			} else {
				// Checks state of numlock to see if middle keys will send e0 or not, and adjust the return code as necessary 
				if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) {
					stroke.code = newCode(stroke.code);
				}

				if (stroke != lshiftE0 && lastStroke != lshiftE0) { // Some keys use the E0 lshift to operate, this ignores that key.
					std::string keyName = returnKeycode(stroke.code);
					std::cout << stroke.code << " | " << keyName << std::endl;
					sendto(clientSock, keyName.c_str(), keyName.length(), 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
				}

				if (lastStroke == lshiftE0) {
					std::string keyName = returnKeycode(stroke.code + 100);
					std::cout << stroke.code + 100 << " | " << keyName << std::endl;
					sendto(clientSock, keyName.c_str(), keyName.length(), 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
				}
			}
			lastStroke = stroke;

			/*
			* Example of how to change keystrokes
			* if (stroke.code == 20) stroke.code = 21;
			*
			* How to send keystrokes as normal; Without this it doesnt let the system interpreter actually get keystrokes
			interception_send(context, device, (InterceptionStroke*)&stroke, 1);
			*/
		}
		else if (deviceID != secondDevice) {
			// Send normal keystrokes for other devices.
			interception_send(context, device, (InterceptionStroke*)&stroke, 1);
		}
		// Temp way to stop program incase it breaks.
		if (stroke.code == esc) break;
	}
	sendto(clientSock, "FATALERROR", 10, 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
	closesocket(clientSock);
	WSACleanup();
	interception_destroy_context(context);
	return 0;
}


int main() {
	test(getDevID());
}
