#pragma once

#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    private:

      bool stoped = false;
      unsigned short port = 27015;
      std::function<std::vector<char>(const std::vector<char>&)> receiveHandler = {};

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      SocketServer() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setReceiveHandler(const std::function<std::vector<char>(const std::vector<char>&)>& function);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setReceiveHandler(std::vector<char>(T::*method)(const std::vector<char>&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void start();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void stop();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      ~SocketServer() = default;

  };

}

namespace exqudens {

  template<class T>
  void SocketServer::setReceiveHandler(std::vector<char>(T::*method)(const std::vector<char>&), void* object) {
    receiveHandler = std::bind(method, reinterpret_cast<T*>(object), std::placeholders::_1);
  }

}
