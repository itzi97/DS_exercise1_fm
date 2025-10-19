#include "clientManager.h"
#include "utils.h"

void clientManager::resolveClientMessages(int clientId) {
  vector<unsigned char> buffer;

  bool exitFM = false;

  do {
    // receive a packet from client
    recvMSG(clientId, buffer);
    msgType_t type = unpack<msgType_t>(buffer);

    switch (type) {
    case ls: { // TODO
      cout << "[CLIENT " << clientId << "] Requested ls" << endl;
    } break;
    case lls: { // TODO
      cout << "[CLIENT " << clientId << "] Requested lls" << endl;
    } break;
    case upload: { // TODO
      cout << "[CLIENT " << clientId << "] Requested upload" << endl;
    } break;
    case download: { // TODO
      cout << "[CLIENT " << clientId << "] Requested download" << endl;
    } break;
    case exit: { // TODO
      cout << "[CLIENT " << clientId << "] Requested exit" << endl;

      exitFM = true;
    } break;
      buffer.clear();
      pack(buffer, ack);
      sendMSG(clientId, buffer);
    }

  } while (!exit);

  closeConnection(clientId);
}
