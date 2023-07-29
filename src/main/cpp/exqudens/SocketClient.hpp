#pragma once

#include <cstddef>
#include <vector>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketClient {

    private:

      unsigned short port = 27015;

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES SocketClient() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES SocketClient& setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES size_t sendData(const std::vector<char>& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES ~SocketClient() = default;

  };

}
