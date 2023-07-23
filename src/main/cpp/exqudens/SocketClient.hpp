#pragma once

#include <cstddef>
#include <vector>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketClient {

    private:

      unsigned short port = 27015;

    public:

      SocketClient() = default;

      SocketClient& setPort(const unsigned short& value);

      size_t sendData(const std::vector<char>& value);

      ~SocketClient() = default;

  };

}
