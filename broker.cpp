#include "msgType.h"
#include "utils.h"
#include <iostream>
#include <limits>
#include <map>
#include <thread>
#include <vector>

using namespace std;

// Info stored for each registered server
struct ServerInfo {
	string ip;
	int connCount;    // number of clients assigned to this server
	int brokerConnId; // connection id on the broker side
};

// keyed by broker connection id
static std::map<int, ServerInfo> serversByConnId;
// clientConnId -> server brokerConnId
static std::map<int, int> clientAssignment;

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

		// Read and handle connection message
		buffer.clear();
		recvMSG(connId, buffer);

		// If message is empty, just close connection
		if (buffer.size() < 1) {
			cout << "[BROKER] Received empty message from connection " << connId
			     << endl;
			closeConnection(connId);
			continue;
		}

		// Unpack message type
		auto type = unpack<fm::msgType_t>(buffer);
		switch (type) {
		case fm::RegisterServer: {
			// Expect server to send its IP string after the message type.
			// Layout: [msgType][long int ipLen][ip bytes]
			if (buffer.size() < (int)sizeof(long int)) {
				cout << "[BROKER] RegisterServer from " << connId << " without IP"
				     << endl;
				closeConnection(connId);
				break;
			}

			// unpack ip length and ip string
			string ip;
			ip.resize(unpack<long int>(buffer));
			unpackv(buffer, (char *)ip.data(), ip.size());

			// store server info (no mutex - broker main loop is single-threaded)
			ServerInfo si;
			si.ip = ip;
			si.connCount = 0;
			si.brokerConnId = connId;
			serversByConnId[connId] = si;

			cout << "[BROKER] Registered server " << connId << " with IP " << ip
			     << " (total servers: " << serversByConnId.size() << ")" << endl;

			// respond with ack
			buffer.clear();
			pack(buffer, fm::ack);
			sendMSG(connId, buffer);

		} break;
		case fm::RegisterClient: {
			// A client requests an active server IP.
			// Choose the server with the smallest connCount (simple load balancing).
			int chosenServerConnId = -1;
			string chosenServerIP;

			if (serversByConnId.empty()) {
				chosenServerConnId = -1;
			} else {
				// find server with minimum connCount
				int minCount = std::numeric_limits<int>::max();
				for (auto &kv : serversByConnId) {
					if (kv.second.connCount < minCount) {
						minCount = kv.second.connCount;
						chosenServerConnId = kv.first;
					}
				}
				if (chosenServerConnId != -1) {
					chosenServerIP = serversByConnId[chosenServerConnId].ip;
					// record assignment (increment the server's connection counter)
					serversByConnId[chosenServerConnId].connCount++;
					clientAssignment[connId] = chosenServerConnId;
				}
			}

			// If no server available, respond with zero-length IP and close
			// connection
			if (chosenServerConnId == -1) {
				cout << "[BROKER] No servers available for client " << connId << endl;
				buffer.clear();
				// pack a zero-length IP as indicator
				pack(buffer, (long int)0);
				pack(buffer, fm::ack);
				sendMSG(connId, buffer);
				closeConnection(connId);
			} else {
				// send the chosen server IP back to the client:
				// [long int ipLen][ip bytes][ack]
				buffer.clear();
				pack(buffer, (long int)chosenServerIP.size());
				if (!chosenServerIP.empty())
					packv(buffer, (char *)chosenServerIP.data(), chosenServerIP.size());
				pack(buffer, fm::ack);
				sendMSG(connId, buffer);

				cout << "[BROKER] Assigned server " << chosenServerConnId << " (IP "
				     << chosenServerIP << ") to client " << connId
				     << " (server now has "
				     << serversByConnId[chosenServerConnId].connCount << " clients)"
				     << endl;
			}
		} break;
		case fm::Ping: {
			// Respond with ack
			cout << "[BROKER] Received Ping message from " << connId << endl;
			buffer.clear();
			pack(buffer, fm::ack);
			sendMSG(connId, buffer);
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
