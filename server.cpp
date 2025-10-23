#include "clientManager.h"
#include "msgTypes.h"
#include "utils.h"
#include <iostream>
#include <thread>

#include <arpa/inet.h>
#include <cstring>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {

	// Get local IP
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;
	// strcpy(ifr.ifr_name, "wlp9s0f0"); // Change to your network interface
	strcpy(ifr.ifr_name, "ens5"); // Change to your network interface

	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	char ip[INET_ADDRSTRLEN];
	strcpy(ip, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	std::cout << "[SERVER] Private IP Address: " << ip << std::endl;

	cout << "[SERVER] opening port " << fmInfo::SERVER_PORT << endl;
	int serverPortId = initServer(fmInfo::SERVER_PORT);
	cout << "[SERVER] initialized on port " << fmInfo::SERVER_PORT << endl;

	// Start loop for client conections
	bool exit = false;

	do {
		// TODO: Check client
		while (!checkClient())
			usleep(100);

		int clientId = getLastClientID();
		cout << "[SERVER] Client " << clientId << " connected" << endl;

		thread *th = new thread(clientManager::resolveClientMessages, clientId);

		// Detach the thread and free the pointer to avoid leaking the thread
		// object.
		th->detach();
		delete th;

		// TODO: Remove after function done
	} while (!exit);

	close(serverPortId);
	cout << "[SERVER] connection closed" << endl;

	return 0;
}
