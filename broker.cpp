// Itziar Morales Rodríguez
#include "msgTypes.h"
#include "utils.h"
#include <iostream>
#include <map>
#include <thread>

#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <cstring>
#include <limits>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

// TODO: Add ping command

using namespace std;

struct ServerData {
	int connId;
	vector<int> connectedClients;
};

// Registered clients and servers
static inline map<string, ServerData> registeredServers;

void pingConnection(int connId, bool isServer) {
	// Implements pinging the connection to keep it alive
	vector<unsigned char> buffer;
	bool connected = true;

	do {
		usleep(100000); // Ping every 10 second

		// send ping msg
		pack(buffer, fmInfo::Ping);
		sendMSG(connId, buffer);

		// receive ack from connection
		buffer.clear();
		try {
			recvMSG(connId, buffer);
			if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::ack)
				connected = false;
		} catch (const runtime_error &e) {
			connected = false;
			cout << "[BROKER] ping to connection " << connId
			     << " failed: " << e.what() << endl;
			break;
		}

		cout << "[BROKER] pinged connection " << connId
		     << (connected ? " successfully" : " failed") << endl;

	} while (connected);

	// Remove connection rom registered servers or client
	if (isServer) {
		// Find server by connId
		string serverIP;
		for (auto it : registeredServers) {
			if (it.second.connId == connId) {
				serverIP = it.first;
				break;
			}
		}
		if (!serverIP.empty()) {
			registeredServers.erase(serverIP);
			cout << "[SERVER " << connId
			     << "] disconnected and removed from "
			        "registered servers"
			     << endl;
		}
	} else {
		// Find client in all servers
		for (auto &it : registeredServers) {
			auto &clients = it.second.connectedClients;
			auto cit = find(clients.begin(), clients.end(), connId);
			if (cit != clients.end()) {
				clients.erase(cit);
				cout << "[CLIENT " << connId
				     << "] disconnected and removed from server " << it.first << endl;
				break;
			}
		}
	}

	return;
}

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

		bool isServer;

		switch (type) {

		case fmInfo::RegisterServer: {
			cout << "[SERVER " << connId << "] Registering connection" << endl;
			// unpack IP
			string serverIP;
			serverIP.resize(unpack<long int>(buffer));
			unpackv(buffer, (char *)serverIP.data(), serverIP.size());
			buffer.clear();

			// Register server
			registeredServers[serverIP] =
			  ServerData{.connId = connId, .connectedClients = vector<int>()};

			// Set to is server
			isServer = true;

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

			// clear buffer
			buffer.clear();

			pack(buffer, serverIP.size());
			packv(buffer, serverIP.data(), serverIP.size());

			registeredServers[serverIP].connectedClients.push_back(connId);

			// Set to not server
			isServer = false;
		}
		}

		pack(buffer, fmInfo::ack);
		sendMSG(connId, buffer);

		// closeConnection(connId);

		thread *th = new thread(pingConnection, connId, isServer);

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
