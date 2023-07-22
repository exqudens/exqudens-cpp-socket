#pragma once

#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    private:

      unsigned int port = 27015;
      size_t clientSocket = (size_t) (~0);
      std::function<void(const std::vector<char>&)> receiveFunction = {};

    public:

      SocketServer() = default;

      SocketServer& setPort(const unsigned int& value);

      SocketServer& setReceiveFunction(const std::function<void(const std::vector<char>&)>& value);

      void start();

      void stop();

      ~SocketServer() = default;

  };

}
