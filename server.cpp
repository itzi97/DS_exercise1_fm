#include "clientManager.h"
#include "utils.h"
#include <iostream>
#include <thread>

using namespace std;

#define BROKER_IP "127.0.0.1"
#define BROKER_PORT 42069

#define SERVER_PORT 1067

int main(int argc, char **argv) {

	// Initialize server and register to broker
	cout << "[SERVER] Registering to broker at " << BROKER_IP << ":"
	     << BROKER_PORT << endl;

	int brokerConnId = initClient(BROKER_IP, BROKER_PORT);
	if (brokerConnId < 0) {
		cout << "[SERVER] Failed to connect to broker at " << BROKER_IP << ":"
		     << BROKER_PORT << endl;
		return 1;
	}

	bool exit = false;

	cout << "[SERVER] opening port " << SERVER_PORT << endl;
	int serverPortId = initServer(SERVER_PORT);
	cout << "[SERVER] initialized on port " << SERVER_PORT << endl;

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

	return 0;
}
