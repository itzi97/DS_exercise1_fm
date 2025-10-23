#include "clientManager.h"
#include "filemanager.h"
#include "msgTypes.h"
#include "utils.h"

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
	pack(buffer, fmInfo::FMDestructor);
	sendMSG(serverId, buffer);

	// receive ack from server
	buffer.clear();
	recvMSG(serverId, buffer);
	if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	// remove from clientConnections map
	clientManager::clientConnections.erase(itmap);

	// close connection
	closeConnection(serverId);
	cout << "Connection " << serverId << " closed." << endl;
}

// TODO: Test if it works
FileManager::FileManager(string path) {
	// Establish connection to server
	auto serverConn =
	  initClient(std::string(fmInfo::SERVER_IP), fmInfo::SERVER_PORT);
	int serverId = serverConn.serverId;

	vector<unsigned char> buffer;

	// pack type
	pack(buffer, fmInfo::FMConstructor);

	// pack path (use long int to match unpack<long int> on the other side)
	pack(buffer, (long int)path.size());
	packv(buffer, path.data(), path.size());

	// send type and path to server
	sendMSG(serverId, buffer);

	// receive ack from server
	buffer.clear();
	recvMSG(serverId, buffer);
	if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	clientManager::clientConnections[this] = serverId;
}

// TODO
vector<string> FileManager::listFiles() {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send listFiles msg
	pack(buffer, fmInfo::FMListFilesF);
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
	if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	return fileList;
}

// TODO
void FileManager::readFile(string fileName, vector<unsigned char> &data) {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send readFile msg
	pack(buffer, fmInfo::FMReadFileF);

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
	if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	cout << "File " << fileName << " read from server." << endl;
}

// TODO
void FileManager::writeFile(string fileName, vector<unsigned char> &data) {
	int serverId = clientManager::clientConnections[this];
	vector<unsigned char> buffer;

	// send writeFile msg
	pack(buffer, fmInfo::FMWriteFileF);

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
	if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::ack)
		cout << "ERROR " << __FILE__ << " " << __LINE__ << endl;

	cout << "File " << fileName << " written to server." << endl;
}
