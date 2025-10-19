#include "clientManager.h"
#include "utils.h"
#include <iostream>
#include <list>
#include <string>
#include <thread>

using namespace std;

void recvMessagesFromServer(int serverId) {
  // TODO: Actually do something or KYS
  cout << "Hello from client!" << endl;

  vector<unsigned char> buffer;

  do {
    recvMSG(serverId, buffer);
    clientManager::msgType_t type = unpack<clientManager::msgType_t>(buffer);

    switch (type) {
    case clientManager::ack: {
      cout << "Received ACK from server" << endl;
    } break;
    default: {
      cout << "ERROR " << __FILE__ << ":" << __LINE__
           << " - Unknown message type received from server: " << type << endl;
    }
    }

  } while (false);
  return;
}

int main(int argc, char **argv) {
  // TODO: Interface for client functionality

  cout << "Connecting to server on 1067" << endl;
  auto serverConnection = initClient("127.0.0.1", 1067);
  cout << "Connected to server: " << serverConnection.serverId << endl;

  thread *th = new thread(recvMessagesFromServer, serverConnection.serverId);

  closeConnection(serverConnection.serverId);
  cout << "Connection closed" << endl;
  return 0;
}
