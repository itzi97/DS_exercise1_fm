#include "clientManager.h"
#include "utils.h"
#include <iostream>
#include <thread>

#include <arpa/inet.h> // this is for inet_ntoa
#include <netdb.h>     // this is for gethostbyname
#include <string>
#include <unistd.h> // this is for gethostname

using namespace std;

int main(int argc, char **argv) {

	// Get local IP

	char host[256];

	// to get local host name
	if (gethostname(host, sizeof(host)) == -1) {
		perror("[SERVER] gethostname failed");
		return 1;
	}

	cout << "[SERVER] Host Name: " << host << endl;

	// to get host information
	hostent *he = gethostbyname(host);
	if (he == nullptr) {
		cerr << "[SERVER] gethostbyname failed" << endl;
		return 1;
	}

	// to get and print the first IP address
	in_addr *addr = (in_addr *)he->h_addr_list[0];
	cout << "[SERVER] IP Address: " << inet_ntoa(*addr) << endl;

	bool exit = false;

	cout << "Server opening port 1067" << endl;
	int serverPortId = initServer(1067);
	cout << "Server initialized on port 1067" << endl;

	do {
		// TODO: Check client
		while (!checkClient())
			usleep(100);

		int clientId = getLastClientID();
		cout << "Client " << clientId << " connected" << endl;

		thread *th = new thread(clientManager::resolveClientMessages, clientId);

		// Detach the thread and free the pointer to avoid leaking the thread
		// object.
		th->detach();
		delete th;

		// TODO: Remove after function done
	} while (!exit);

	close(serverPortId);

	return 0;
}
