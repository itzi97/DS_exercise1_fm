#pragma once

#include "utils.h"
#include <string>

using namespace std;

class clientManager {

public:
  typedef enum { ls, lls, upload, download, exit, ack } msgType_t;

  static inline map<string, int> connectionIds;

  static void resolveClientMessages(int clientId);
};
