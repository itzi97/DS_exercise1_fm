#include "clientManager.h"
#include "msgType.h"
#include "utils.h"
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char **argv) {

	// Initialize server and register to broker
	cout << "[SERVER] Registering to broker at " << BROKER_IP << ":"
	     << BROKER_PORT << endl;

	auto brokerConnId = initClient(BROKER_IP, BROKER_PORT);
	if (!brokerConnId.alive) {
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
