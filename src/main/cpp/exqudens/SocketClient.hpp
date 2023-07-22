#pragma once

#include <vector>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketClient {

    private:

      unsigned int port = 27015;

    public:

      SocketClient() = default;

      SocketClient& setPort(const unsigned int& value);

      int sendData(const std::vector<char>& value);

      ~SocketClient() = default;

  };

}
