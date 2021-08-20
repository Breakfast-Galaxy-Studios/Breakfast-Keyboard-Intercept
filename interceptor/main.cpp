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
// This program wont run if the file does not exist.
int getDevID() {
	std::string path = getenv("APPDATA");
	path += "\\BGS-Soundboard\\interception.properties";
	AutoPtr<PropertyFileConfiguration> config = new PropertyFileConfiguration(path);
	return config->getInt("devID");
}

// Uses tdp to listen for things on port localhost::55556
// If it gets anything it closes the program
bool closeProgram(std::string localhost) {
	// Socket is defined same was as in main, just with a different port
	struct sockaddr_in recieveAddr;
	recieveAddr.sin_family = AF_INET;
	InetPton(AF_INET, localhost.c_str(), &recieveAddr.sin_addr.s_addr);
	recieveAddr.sin_port = htons(55556);
	int serverAddrSize = sizeof(recieveAddr);
	SOCKET socketfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(socketfd, (struct sockaddr*)&recieveAddr, sizeof(recieveAddr)); // Bind the new socket to the specified port & address
	listen(socketfd, 20); // Tell the os that we are listening on this port
	accept(socketfd, NULL, NULL); // If any data is on the port, continue this function. accept() is a blocking function
	exit(100); // Close the program, with exit code 100 to specify it was closed via tcp
}

int main() {
	int ID = getDevID();
	std::string localhost = "127.0.0.1";
	InterceptionContext context;
	InterceptionDevice device;
	InterceptionKeyStroke stroke, lastStroke;
	short int timesPressed = 0;
	wchar_t hardware_id[500];
	// E0 form of lshift, certain keys use this
	InterceptionKeyStroke lshiftE0 = { 42, INTERCEPTION_KEY_E0 };

	
	// Main UDP Socket
	WSADATA wsaData;										
	WSAStartup(0x0202, &wsaData);										
	struct sockaddr_in serverAddr;										
	serverAddr.sin_family = AF_INET;									// Define that this socketaddr is IPv4
	InetPton(AF_INET, localhost.c_str(), &serverAddr.sin_addr.s_addr);	// Set the socketaddr IP address, in this case 127.0.0.1 (localhost)
	serverAddr.sin_port = htons(55555);									// Set the socketaddr port
	int clientSock = socket(PF_INET, SOCK_DGRAM, 0);					// Finally create the socket
	// Open an async thread to listen for things from Soundboard, while still letting this run normally
	std::async(std::launch::async, closeProgram, localhost);

	// Things needed to actually listen for the keystrokes
	raise_process_priority();
	context = interception_create_context();

	// Listen for only keyboards, and only keydown & E0 events
	interception_set_filter(context, interception_is_keyboard, INTERCEPTION_FILTER_KEY_DOWN | INTERCEPTION_FILTER_KEY_E0); 
	while (interception_receive(context, device = interception_wait(context), (InterceptionStroke*)&stroke, 1) > 0) {
		// Get the deviceID that the keystroke came from
		size_t deviceID = interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));
		const int secondDevice = ID;
		if (deviceID == secondDevice) {
			if (stroke.code == numlock) {
				// Let numlock change systemwide, otherwise the keyboard will keep track of it's own numlock, meaning it can vary from system wide numlock
				interception_send(context, device, (InterceptionStroke*)&stroke, 1);
			} else {

				// Checks state of numlock to see if middle keys will send E0 or not, and adjust the return code as necessary 
				if ((GetKeyState(VK_NUMLOCK) & 0x0001) == 0) {
					// In the case of the middle of keyboard keys, add still add 100 if numlock is off
					// Without this, these keys will report as numpad keys
					stroke.code = newCode(stroke.code);
				}

				// Some keys use the E0 lshift to operate, this ignores that key.
				if (stroke != lshiftE0 && lastStroke != lshiftE0) { 
					std::string keyName = returnKeycode(stroke.code);
					std::cout << stroke.code << " | " << keyName << std::endl;
					sendto(clientSock, keyName.c_str(), keyName.length(), 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
				}

				// If lastStroke was E0 lshift, add 100 to the code to get the actual key that was pressed.
				if (lastStroke == lshiftE0) {
					std::string keyName = returnKeycode(stroke.code + 100);
					std::cout << stroke.code + 100 << " | " << keyName << std::endl;
					sendto(clientSock, keyName.c_str(), keyName.length(), 0, (LPSOCKADDR)&serverAddr, sizeof(struct sockaddr));
				}
			}
			// Monitor "last" keystroke, mainly used to see if E0 lshift is pressed.
			lastStroke = stroke;
		}
		else if (deviceID != secondDevice) {
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
