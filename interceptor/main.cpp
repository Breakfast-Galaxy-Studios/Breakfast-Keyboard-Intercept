#include <intercept.h>
#include <utils.h>
#include <iostream>
#include <main.h>
#include <thread>
#include <future>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#pragma comment(lib, "Ws2_32.lib")
using Poco::AutoPtr;
using Poco::Util::PropertyFileConfiguration;
// Special scancodes that need to be quickly accessed
enum ScanCode {
	esc = 1,
	numlock = 69
};

// Allows the comparison of InterceptionKeyStroke
bool operator==(const InterceptionKeyStroke& first,
	const InterceptionKeyStroke& second) {
	return first.code == second.code && first.state == second.state;
}
bool operator!=(const InterceptionKeyStroke& first,
	const InterceptionKeyStroke& second) {
	return first.code != second.code || first.state != second.state;
}

// Get device id from %appdata%/BGS-Soundboard/interception.properties
// This file will be created by Soundboard
// This program will throw an exception and close if it doesnt exist
int getDevID() {
	std::string path = getenv("APPDATA");
	path += "\\BGS-Soundboard\\interception.properties";
	try {
		AutoPtr<PropertyFileConfiguration> config = new PropertyFileConfiguration(path);
		return config->getInt("devID");
	} catch (...){ // Catch all exceptions b/c poco throws a lot of exceptions when it cant find a file
		MessageBox(0, "Failed to find the interception.properties file.", "Interception Error", MB_OK);
		exit(30);
	}
}

// If it gets anything it closes the program
bool closeProgram(std::string localhost, int clientSock, InterceptionContext context) {
	// Socket is defined same was as in main, just with a different port
	struct sockaddr_in recieveAddr;
	recieveAddr.sin_family = AF_INET;
	InetPton(AF_INET, localhost.c_str(), &recieveAddr.sin_addr.s_addr);
	recieveAddr.sin_port = htons(58585);
	int serverAddrSize = sizeof(recieveAddr);
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	int result = bind(socketfd, (struct sockaddr*)&recieveAddr, serverAddrSize); // Bind the new socket to the specified port & address
	if (result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(socketfd);
		WSACleanup();
		return 1;
	}
	listen(socketfd, 20);											// Tell the os that we are listening on this port
	accept(socketfd, NULL, NULL);									// If any data is on the port, continue this function. accept() is a blocking function+
	closesocket(socketfd);
	closesocket(clientSock);
	WSACleanup();
	exit(100);														// Close the program, with exit code 100 to specify it was closed via tcp
}

int main() {
	int ID = getDevID();
	std::string localhost = "127.0.0.1";
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionKeyStroke stroke, lastStroke;
	wchar_t hardware_id[500];
	// E0 form of lshift, certain keys use this
	InterceptionKeyStroke lshiftE0 = { 42, INTERCEPTION_KEY_E0 };
	InterceptionKeyStroke ctrlE1 = { 29, INTERCEPTION_KEY_E1 };
	
	// Main UDP Socket
	WSADATA wsaData;										
	WSAStartup(0x0202, &wsaData);	
	// Create the main port
	struct sockaddr_in serverAddr;										
	serverAddr.sin_family = AF_INET;									// Define that this socketaddr is IPv4
	InetPton(AF_INET, localhost.c_str(), &serverAddr.sin_addr.s_addr);	// Set the socketaddr IP address, in this case 127.0.0.1 (localhost)
	serverAddr.sin_port = htons(55555);									// Set the socketaddr port

	// Create the port used for listening to keybinds
	struct sockaddr_in recordAddr;
	recordAddr.sin_family = AF_INET;
	InetPton(AF_INET, localhost.c_str(), &recordAddr.sin_addr.s_addr);
	recordAddr.sin_port = htons(55556);
	// Finally create the UDP socket
	int clientSock = socket(PF_INET, SOCK_DGRAM, 0);					
	// Open an async thread to listen for things from Soundboard, while still letting this run normally
	auto f = std::async(std::launch::async, closeProgram, localhost, clientSock, context);

	// Things needed to actually listen for the keystrokes
	raise_process_priority();
	context = interception_create_context();

	// Listen for only keyboards, and only keydown & E0 events
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_E0 | INTERCEPTION_FILTER_KEY_E1); 
	while (interception_receive(context, device = interception_wait(context), (InterceptionStroke*)&stroke, 1) > 0) {
		// Get the deviceID that the keystroke came from
		size_t deviceID = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));
		const int secondDevice = ID;
		char buf[256];
		if (deviceID == secondDevice) {
			std::cout << stroke.code << std::endl;
			// Keys f13-f24 & middle keys (Scroll Lock, insert, delete, etc) all send E0 (disguised as lshift) when numlock is on. 
			// Pause Break sends code E1 (disguised as ctrl) no matter the circumstances. 
			if (lastStroke == lshiftE0 || lastStroke == ctrlE1) {
				stroke.code = newCode(stroke.code);
			}
			if (stroke.code == numlock) {
				// Let numlock change systemwide, otherwise the keyboard will keep track of it's own numlock, meaning it can vary from system wide numlock
				interception_send(context, device, (InterceptionStroke*)&stroke, 1);
			} else {
				// Checks state of numlock to see if middle keys will send E0 or not, and adjust the return code as necessary 
				if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) {
					// In the case of the middle of keyboard keys, still add 100 if numlock is off; Without this, these keys will report as numpad keys.
					stroke.code = newCode(stroke.code);
				}

				// Some keys use the E0 lshift to operate, this ignores that key.
				// Pause uses key E1, this ignores that too.
				if (stroke != lshiftE0 && stroke != ctrlE1) {
					sprintf_s(buf, "%d", returnKeyInt(stroke.code));
					sendto(clientSock, buf, sizeof buf, 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
					sendto(clientSock, buf, sizeof buf, 0, (LPSOCKADDR)&recordAddr, sizeof(struct sockaddr));


					// Used for debugging
					// Send key as string
					// std::string keyName = returnKeyString(stroke.code);
					// std::cout << stroke.code << " | " << keyName << std::endl;
					// sendto(clientSock, (const char*) thingToSend, sizeof thingToSend, 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
					// sendto(clientSock, (const char*) thingToSend, sizeof thingToSend, 0, (LPSOCKADDR)&recordAddr, sizeof(struct sockaddr));
				}
			}
			// Monitor "last" keystroke, used to see if E0 lshift is pressed.
			lastStroke = stroke;
		}
		else {
			// Send normal keystrokes for other devices.
			interception_send(context, device, (InterceptionStroke*)&stroke, 1);
		}
	}

	// Let Soundboard know that the program is closing for some reason:
	sendto(clientSock, "FATALERROR", 10, 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
	// Close and cleanup the sockets
	closesocket(clientSock);
	WSACleanup();
	// Destroy the instance of keyboard interception
	interception_destroy_context(context);
	return 0;
}
