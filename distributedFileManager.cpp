#include "clientManager.h"
#include "filemanager.h"
#include "msgTypes.h"
#include "utils.h"
#include <iostream>
#include <string>

// TODO
FileManager::FileManager() : FileManager("") {}

// TODO: Test if it works
FileManager::~FileManager() {
	// check mapping existence
	auto itmap = clientManager::clientConnections.find(this);
	if (itmap == clientManager::clientConnections.end()) {
		// nothing to do
		return;
	}
	int serverId = itmap->second;
	vector<unsigned char> buffer;

	// send destructor msg
	pack(buffer, FMInfo::FMDestructor);
	sendMSG(serverId, buffer);

	// receive ack from server
	buffer.clear();
	recvMSG(serverId, buffer);
	if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	// remove from clientConnections map
	clientManager::clientConnections.erase(itmap);

	// close connection
	closeConnection(serverId);
	cout << "Connection " << serverId << " closed." << endl;
}

// New behavior: ask broker for an active server IP, then connect to that server
FileManager::FileManager(string path) {
	// Connect to broker and request a server IP
	auto brokerConn = initClient(BROKER_IP, BROKER_PORT);
	if (!brokerConn.alive) {
		cout << "[FM] Failed to connect to broker at " << BROKER_IP << ":"
		     << BROKER_PORT << endl;
		return;
	}

	vector<unsigned char> buffer;

	// Request a server (RegisterClient)
	pack(buffer, FMInfo::RegisterClient);
	sendMSG(brokerConn.serverId, buffer);

	// Receive response: [long ipLen][ip bytes][ack]
	buffer.clear();
	recvMSG(brokerConn.serverId, buffer);

	long int ipLen = unpack<long int>(buffer);
	string serverIP;
	if (ipLen > 0) {
		serverIP.resize(ipLen);
		unpackv(buffer, (char *)serverIP.data(), ipLen);
	} else {
		cout << "[FM] Broker returned no available server" << endl;
		// cleanup broker connection and bail out
		closeConnection(brokerConn.serverId);
		return;
	}

	// check ack
	if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack) {
		cout << "ERROR " << __FILE__ << " " << __LINE__
		     << " ack expected from broker." << endl;
		// proceed anyway
	}

	// close broker connection - we don't need it anymore
	closeConnection(brokerConn.serverId);

	// Establish connection to server using the IP returned by the broker
	auto serverConn = initClient(serverIP, SERVER_PORT);
	if (!serverConn.alive) {
		cout << "[FM] Failed to connect to server at " << serverIP << ":"
		     << SERVER_PORT << endl;
		return;
	}
	int serverId = serverConn.serverId;

	// pack type
	buffer.clear();
	pack(buffer, FMInfo::FMConstructor);

	// pack path (use long int to match unpack<long int> on the other side)
	pack(buffer, (long int)path.size());
	packv(buffer, path.data(), path.size());

	// send type and path to server
	sendMSG(serverId, buffer);

	// receive ack from server
	buffer.clear();
	recvMSG(serverId, buffer);
	if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	clientManager::clientConnections[this] = serverId;
}

// TODO: Test if it works
vector<string> FileManager::listFiles() {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send listFiles msg
	pack(buffer, FMInfo::FMListFilesF);
	sendMSG(serverId, buffer);

	// Clear buffer and receive server message
	buffer.clear();
	recvMSG(serverId, buffer);

	// receive file list from server and return
	vector<string> fileList;
	fileList.resize(unpack<long int>(buffer));
	for (auto &fileName : fileList) {
		fileName.resize(unpack<long int>(buffer));
		unpackv(buffer, (char *)fileName.data(), fileName.size());
	}

	// receive ack
	if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	return fileList;
}

// TODO
void FileManager::readFile(string fileName, vector<unsigned char> &data) {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send readFile msg
	pack(buffer, FMInfo::FMReadFileF);

	// pack file name (use long int)
	pack(buffer, (long int)fileName.size());
	packv(buffer, fileName.data(), fileName.size());

	// send file name to server
	sendMSG(serverId, buffer);

	// receive data from server
	buffer.clear();
	recvMSG(serverId, buffer);

	data.resize(unpack<long int>(buffer));
	unpackv(buffer, (unsigned char *)data.data(), data.size());

	// receive ack from server
	if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	cout << "File " << fileName << " read from server." << endl;
}

// TODO
void FileManager::writeFile(string fileName, vector<unsigned char> &data) {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send writeFile msg
	pack(buffer, FMInfo::FMWriteFileF);

	// pack file name (use long int)
	pack(buffer, (long int)fileName.size());
	packv(buffer, fileName.data(), fileName.size());

	// pack data (use long int)
	pack(buffer, (long int)data.size());
	packv(buffer, data.data(), data.size());

	// send file name and data to server
	sendMSG(serverId, buffer);

	// receive ack from server
	buffer.clear();
	recvMSG(serverId, buffer);
	if (unpack<FMInfo::msgType_t>(buffer) != FMInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	cout << "File " << fileName << " written to server." << endl;
}
