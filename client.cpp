#include "utils.h"
#include <iostream>
#include <list>
#include <string>
#include <thread>

using namespace std;

void recvMessagesFromServer(int serverId) {
  // TODO: Actually do something or KYS
  cout << "Hello from client!" << endl;
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
