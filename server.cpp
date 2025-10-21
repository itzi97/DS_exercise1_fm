#include "clientManager.h"
#include "utils.h"
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char **argv) {
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
