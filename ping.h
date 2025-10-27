// Itziar Morales Rodríguez
#pragma once

#include "msgTypes.h"
#include "utils.h"

#include <vector>

using namespace std;

void pingHandler(int connId) {
	// Checks pings from broker
	vector<unsigned char> buffer;

	do {
		usleep(1000); // Check for pings every 1 second

		recvMSG(connId, buffer);
		if (unpack<fmInfo::msgType_t>(buffer) != fmInfo::Ping)
			exit(1);

		buffer.clear();

		pack(buffer, fmInfo::ack);
		sendMSG(connId, buffer);
		buffer.clear();
	} while (true);
}
