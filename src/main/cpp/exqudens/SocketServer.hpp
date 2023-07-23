#pragma once

#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    private:

      unsigned short port = 27015;
      std::function<std::vector<char>(const std::vector<char>&)> receiveFunction = {};

    public:

      SocketServer() = default;

      SocketServer& setPort(const unsigned short& value);

      SocketServer& setReceiveFunction(const std::function<std::vector<char>(const std::vector<char>&)>& value);

      void start();

      void stop();

      ~SocketServer() = default;

  };

}
