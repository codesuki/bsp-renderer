#include "messenger.hpp"

#include <map>
#include <vector>

#include "message.hpp"

namespace messenger {
namespace {

// TODO: change to std::array using MESSAGE enum as indices
std::map<MESSAGE, std::vector<CallbackType>> receivers_;
}

int RegisterReceiver(MESSAGE msg, CallbackType callback) {
  receivers_[msg].push_back(callback);

  return receivers_[msg].size();
}

int UnregisterReceiver() { return 0; }

int BroadcastMessage(MESSAGE type, Message *msg) {
  for (CallbackType receiver : receivers_[type]) {
    receiver(msg);
  }

  return 0;
}
}
