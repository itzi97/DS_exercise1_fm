#include "clientManager.h"
#include "msgType.h"
#include "utils.h"
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

// Server pool stored in a map
static vector<int> serverPool; // FIFO pool of server connection Ids

static void forwardLoop(int fromId, int toId) {
	vector<unsigned char> buffer;
	while (true) {
		// Receive message from fromId
		recvMSG(fromId, buffer);
		// If message is empty, close both connections and exit loop
		if (buffer.size() < 1) {
			cout << "[BROKER] Closing connections " << fromId << " and " << toId
			     << endl;
			closeConnection(fromId);
			closeConnection(toId);
			break;
		}
		// Forward message to toId
		sendMSG(toId, buffer);

		// Clear buffer after sending message
		buffer.clear();
	}
}

static void proxyPair(int clientId, int serverId) {

	// Pair clientId with serverId and proxy both directions
	// Uses two detached threads that forwards data in both directions
	thread t1(forwardLoop, clientId, serverId);
	thread t2(forwardLoop, serverId, clientId);

	// Detach threads and free pointers
	t1.detach();
	t2.detach();

	cout << "[BROKER] Started proxy between client " << clientId << " and server "
	     << serverId << endl;
}

int main(int argc, char **argv) {

	bool exit = false;

	cout << "[BROKER] opening port " << BROKER_PORT << endl;
	int brokerPortId = initServer(BROKER_PORT);

	if (brokerPortId < 0) {
		cout << "[BROKER] Failed to initialize broker on port " << BROKER_PORT
		     << endl;
		return 1;
	}

	cout << "[BROKER] listening with Id " << brokerPortId << endl;

	vector<unsigned char> buffer;

	while (true) {
		// Wait for pending connection
		while (!checkClient())
			usleep(1000); // 1 ms

		// Get connection Id
		int connId = getLastClientID();
		cout << "[BROKER] Client connected with Id " << connId << endl;

		// TODO: Read and handle connection message
		recvMSG(connId, buffer);

		// If message is empty, just close connection
		if (buffer.size() < 1) {
			cout << "[BROKER] Received empty message from connection " << connId
			     << endl;
			closeConnection(connId);
			continue;
		}

		// Unpack message type if buffer isn't empty
		// 1. If RegisterServer, add to server pool
		// 2. If RegisterClient, assign to a server from the pool
		// - If no server available, respond with error
		// 3. If Ping, respond with ack
		auto type = unpack<fm::msgType_t>(buffer);
		switch (type) {
		case fm::RegisterServer: {
			// Add server to pool
			serverPool.push_back(connId);
			cout << "[BROKER] Registered server " << connId
			     << "(pool size: " << serverPool.size() << ")" << endl;
		} break;
		case fm::RegisterClient: {
			// If server pool empty, respond with error and close connection
			if (serverPool.empty()) {
				cout << "[BROKER] No servers available for client " << connId << endl;
				closeConnection(connId);
			} else {
				int serverId = serverPool.back();
				serverPool.pop_back();
				cout << "[BROKER] Assigned server " << serverId << " to client "
				     << connId << endl;
				// TODO: Create function to send serverId to client
			}
		} break;
		case fm::Ping: {
			// TODO:
			cout << "[BROKER] Received Ping message from " << connId << endl;
		} break;
		default: {
			cout << "[BROKER] Received unknown message type from " << connId << endl;
			closeConnection(connId);
		}
		}
	}

	close(brokerPortId);

	return 0;
}
