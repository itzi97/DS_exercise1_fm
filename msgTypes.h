// Itziar Morales Rodríguez
#pragma once

#include <string_view>

using namespace std;

namespace fmInfo {

inline constexpr string_view BROKER_IP = "172.31.22.54";
inline constexpr int BROKER_PORT = 42069;

inline constexpr int SERVER_PORT = 1067;

// TODO: Make server get its own IP
// inline constexpr string_view SERVER_IP = "127.0.0.1";

typedef enum {

	// Broker functions
	RegisterClient,
	RegisterServer,
	RegisterError,
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

}; // namespace fmInfo
