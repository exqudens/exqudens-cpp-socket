#pragma once

#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    private:

      bool stoped = false;
      unsigned short port = 27015;
      std::function<std::vector<char>(const std::vector<char>&)> receiveFunction = {};

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES SocketServer() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES SocketServer& setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES SocketServer& setReceiveFunction(const std::function<std::vector<char>(const std::vector<char>&)>& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES void start();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES void stop();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES ~SocketServer() = default;

  };

}
