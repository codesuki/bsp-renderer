#include "Messenger.h"

#include <map>
#include <vector>

namespace messenger
{
  namespace 
  {
    std::map<MESSAGE, std::vector<CallbackType>> keymap_;
  }

  int RegisterReceiver(MESSAGE msg, CallbackType callback) 
  {
    std::vector<CallbackType>& receivers = keymap_[msg];
    receivers.push_back(callback);
  }

  int UnregisterReceiver()
  {
  }

  int BroadcastMessage(MESSAGE msg)
  {
    const std::vector<CallbackType>& receivers = keymap_[msg];

    for (CallbackType receiver : receivers) 
    {
      receiver(true);
    }
  }
}
