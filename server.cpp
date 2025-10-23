#include "clientManager.h"
#include "msgTypes.h"
#include "utils.h"
#include <arpa/inet.h> // Needed to get IP
#include <iostream>
#include <thread>

using namespace std;

int main(int argc, char **argv) {

	// Initialize server and register to broker
	cout << "[SERVER] Registering to broker at " << BROKER_IP << ":"
	     << BROKER_PORT << endl;

	// Error in case broker is not alive
	auto brokerConn = initClient(BROKER_IP, BROKER_PORT);
	if (!brokerConn.alive) {
		cout << "[SERVER] Failed to connect to broker at " << BROKER_IP << ":"
		     << BROKER_PORT << endl;
		return 1;
	}

	// determine local IP used for the connection (private IP)
	struct sockaddr_in localAddr;
	socklen_t addrLen = sizeof(localAddr);
	if (getsockname(brokerConn.socket, (struct sockaddr *)&localAddr, &addrLen) !=
	    0) {
		cout << "[SERVER] Warning: could not get local socket name, registering "
		        "127.0.0.1"
		     << endl;
		localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	string localIP = inet_ntoa(localAddr.sin_addr);

	// send RegisterServer message + IP
	{
		vector<unsigned char> buffer;
		pack(buffer, FMInfo::RegisterServer);
		pack(buffer, (long int)localIP.size());
		if (!localIP.empty())
			packv(buffer, (char *)localIP.data(), localIP.size());
		sendMSG(brokerConn.serverId, buffer);

		// wait ack
		buffer.clear();
		recvMSG(brokerConn.serverId, buffer);
		if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack) {
			cout << "[SERVER] Broker did not ack RegisterServer" << endl;
			// continue anyway
		} else {
			cout << "[SERVER] Broker acknowledged registration (IP: " << localIP
			     << ")" << endl;
		}
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
