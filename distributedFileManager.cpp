#include "clientManager.h"
#include "filemanager.h"
#include "utils.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1337

// Helper function to receive ACK from server
void recvACK(int serverId) {
	vector<unsigned char> buffer;
	recvMSG(serverId, buffer);
	if (unpack<clientManager::msgType_t>(buffer) != clientManager::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;
}

// TODO
FileManager::FileManager() : FileManager("") {}

// TODO: Test if it works
FileManager::~FileManager() {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send destructor msg
	pack(buffer, clientManager::FMDestructor);
	sendMSG(serverId, buffer);

	// receive ack from server
	recvACK(serverId);

	// remove from clientConnections map
	clientManager::clientConnections.erase(this);

	// close connection
	closeConnection(serverId);
	cout << "Connection " << serverId << " closed." << endl;
}

// TODO: Test if it works
FileManager::FileManager(string path) {
	// Establish connection to server
	auto serverConn = initClient(SERVER_IP, SERVER_PORT);
	int serverId = serverConn.serverId;

	vector<unsigned char> buffer;

	// pack type
	pack(buffer, clientManager::FMConstructor);

	// pack path
	pack(buffer, path.size());
	packv(buffer, path.data(), path.size());

	// send type and path to server
	sendMSG(serverId, buffer);

	// receive ack from server
	recvACK(serverId);

	clientManager::clientConnections[this] = serverId;
}

// TODO
vector<string> FileManager::listFiles() {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send listFiles msg
	pack(buffer, clientManager::FMListFilesF);
	sendMSG(serverId, buffer);

	// receive ack from server and clear buffer
	recvACK(serverId);
	buffer.clear();

	// receive file list from server and return
	vector<string> fileList;
	fileList.resize(unpack<long int>(buffer));
	for (auto &fileName : fileList) {
		fileName.resize(unpack<long int>(buffer));
		unpackv(buffer, (char *)fileName.data(), fileName.size());
	}

	return fileList;
}

// TODO
void FileManager::readFile(string fileName, vector<unsigned char> &data) {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send readFile msg
	pack(buffer, clientManager::FMReadFileF);

	// pack file name
	pack(buffer, fileName.size());
	packv(buffer, fileName.data(), fileName.size());

	// pack data
	pack(buffer, data.size());
	packv(buffer, data.data(), data.size());

	// send file name and data to server
	sendMSG(serverId, buffer);

	// receive ack from server
	recvACK(serverId);
}

// TODO
void FileManager::writeFile(string fileName, vector<unsigned char> &data) {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send writeFile msg
	pack(buffer, clientManager::FMWriteFileF);

	// pack file name
	pack(buffer, fileName.size());
	packv(buffer, fileName.data(), fileName.size());

	// pack data
	pack(buffer, data.size());
	packv(buffer, data.data(), data.size());

	// send file name and data to server
	sendMSG(serverId, buffer);

	// receive ack from server
	recvACK(serverId);
}
