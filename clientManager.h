#pragma once

#include "filemanager.h"
#include "utils.h"
#include <string>

using namespace std;

class clientManager {

public:
	static inline map<FileManager *, int> clientConnections;
	static inline map<int, FileManager *> clientInstances;

	// TODO: This only works for one client
	static inline bool isConnectionAlive = true;
	static inline bool isClientDisconnected = false;

	static void resolveClientMessages(int clientId);
};
