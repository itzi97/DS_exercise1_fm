#include "clientManager.h"
#include "msgTypes.h"
#include "utils.h"

void clientManager::resolveClientMessages(int clientId) {
	vector<unsigned char> buffer;
	bool exitFM = false;

	do {
		// receive a packet from client
		recvMSG(clientId, buffer);
		FMInfo::msgType_t type = unpack<FMInfo::msgType_t>(buffer);

		switch (type) {

		case FMInfo::FMConstructor: {
			cout << "[CLIENT " << clientId << "] FM Constructor called" << endl;
			// unpack filepath
			string filePath;
			filePath.resize(unpack<long int>(buffer));
			unpackv(buffer, (char *)filePath.data(), filePath.size());

			// instantiate the new class
			clientInstances[clientId] = new FileManager(filePath);

			// clear buffer
			buffer.clear();

		} break;
		case FMInfo::FMDestructor: {
			cout << "[CLIENT " << clientId << "] FM Destructor called" << endl;

			// check existence before deleting to avoid operator[] side-effects
			auto it = clientInstances.find(clientId);
			if (it != clientInstances.end()) {
				delete it->second;
				clientInstances.erase(it);
			}

			exitFM = true;

			buffer.clear();
		} break;
		case FMInfo::FMListFilesF: {
			cout << "[CLIENT " << clientId << "] FM List Files called" << endl;

			// Check instance exists
			auto it = clientInstances.find(clientId);
			vector<string> fileList;
			if (it != clientInstances.end()) {
				fileList = it->second->listFiles();
			} else {
				// no instance: return empty list
				fileList.clear();
			}

			// Clear buffer before packing
			buffer.clear();

			// Send file list to client (use long int to be compatible with
			// unpack<long int>())
			pack(buffer, (long int)fileList.size());
			for (auto &fileName : fileList) {
				pack(buffer, (long int)fileName.size());
				packv(buffer, fileName.data(), fileName.size());
			}

		} break;
		case FMInfo::FMReadFileF: {
			cout << "[CLIENT " << clientId << "] FM Read File called" << endl;

			// get params
			string fileName;
			vector<unsigned char> data;

			// Get file name
			fileName.resize(unpack<long int>(buffer));
			unpackv(buffer, (char *)fileName.data(), fileName.size());

			// Get data (check instance)
			auto it = clientInstances.find(clientId);
			if (it != clientInstances.end()) {
				it->second->readFile(fileName, data);
			} else {
				data.clear();
			}

			// Clear buffer and pack data
			buffer.clear();
			pack(buffer, (long int)data.size());
			packv(buffer, data.data(), data.size());

		} break;
		case FMInfo::FMWriteFileF: {
			cout << "[CLIENT " << clientId << "] FM Write File called" << endl;

			// get params
			string fileName;
			vector<unsigned char> data;

			// Get file name
			fileName.resize(unpack<long int>(buffer));
			unpackv(buffer, (char *)fileName.data(), fileName.size());

			// Get data
			data.resize(unpack<long int>(buffer));
			unpackv(buffer, (unsigned char *)data.data(), data.size());

			// Write file (check instance)
			auto it = clientInstances.find(clientId);
			if (it != clientInstances.end()) {
				it->second->writeFile(fileName, data);
			}

			// Clear buffer
			buffer.clear();

		} break;
		}
		pack(buffer, FMInfo::ack);
		sendMSG(clientId, buffer);

	} while (!exitFM);

	closeConnection(clientId);
}
