#pragma once

#define BROKER_IP "127.0.0.1"
#define BROKER_PORT 42069

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1067

namespace fm {
typedef enum {

	// Broker messages
	RegisterServer,
	RegisterClient,
	Ping,

	// FM Functions
	FMConstructor,
	FMDestructor,
	FMListFilesF,
	FMReadFileF,
	FMWriteFileF,

	// ack
	ack
} msgType_t;
}
