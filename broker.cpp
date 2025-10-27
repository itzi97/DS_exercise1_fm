#include "msgTypes.h"
#include "utils.h"
#include <iostream>
#include <map>
#include <thread>

#include <arpa/inet.h>
#include <cstring>
#include <limits>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

struct ServerData {
	int connId;
	vector<int> connectedClients;
};

// Registered clients and servers
static inline map<string, ServerData> registeredServers;

void registerConnection(int connId) { return; }

int main(int argc, char **argv) {

	cout << "[BROKER] opening port " << fmInfo::BROKER_PORT << endl;
	int brokerPortId = initServer(fmInfo::BROKER_PORT);
	cout << "[BROKER] initialized on port " << fmInfo::BROKER_PORT << endl;

	// Start loop for client & server connections
	bool exit = false;
	vector<unsigned char> buffer;

	do {
		// TODO: Check client & server
		while (!checkClient())
			usleep(100);

		int connId = getLastClientID();

		cout << "[BROKER] " << connId << " connected" << endl;

		// receive a packet from connection
		recvMSG(connId, buffer);
		fmInfo::msgType_t type = unpack<fmInfo::msgType_t>(buffer);

		switch (type) {

		case fmInfo::RegisterServer: {
			cout << "[SERVER " << connId << "] Registering connection" << endl;
			// unpack IP
			string serverIP;
			serverIP.resize(unpack<long int>(buffer));
			unpackv(buffer, (char *)serverIP.data(), serverIP.size());

			// Register server and clear buffer
			registeredServers[serverIP] =
			  ServerData{.connId = connId, .connectedClients = vector<int>()};
			buffer.clear();

		} break;
		case fmInfo::RegisterClient: {
			cout << "[CLIENT " << connId << "] Registering connection" << endl;
			// unpack IP

			// TODO: In case there are no servers available, raise error

			string serverIP;
			int lowestConn = numeric_limits<int>::max();
			for (auto it : registeredServers) {
				if (it.second.connectedClients.size() < lowestConn) {
					lowestConn = it.second.connectedClients.size();
					serverIP = it.first;
				}
			}

			registeredServers[serverIP].connectedClients.push_back(connId);

			// clear buffer
			buffer.clear();
		}
		}

		pack(buffer, fmInfo::ack);
		sendMSG(connId, buffer);

		// closeConnection(connId);

		thread *th = new thread(registerConnection, connId);

		// Detach the thread and free the pointer to avoid leaking the thread
		// object.
		th->detach();
		delete th;

		// TODO: Remove after function done
	} while (!exit);

	close(brokerPortId);
	cout << "[SERVER] connection closed" << endl;

	return 0;
}
