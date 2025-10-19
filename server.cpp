#include "clientManager.h"
#include "utils.h"
#include <iostream>
#include <list>
#include <string>
#include <thread>

using namespace std;

void resolveClientMessages(int clientId) {
  // TODO: Actually do something or KYS
  cout << "Hello from client manager!" << endl;

  // Start buffer and shit for messages
  // vector<unsigned char> buffer;

  closeConnection(clientId);
}

int main(int argc, char **argv) {
  bool exit = false;

  cout << "Server opening port 1067" << endl;
  int serverPortId = initServer(1067);
  cout << "Server initialized on port 1067" << endl;

  do {
    // TODO: Check client
    while (!checkClient())
      usleep(100);

    int clientId = getLastClientID();
    cout << "Client " << clientId << "connected" << endl;

    thread *th = new thread(clientManager::resolveClientMessages, clientId);

    // TODO: Remove after function done
    exit = true;
  } while (!exit);

  close(serverPortId);

  return 0;
}
