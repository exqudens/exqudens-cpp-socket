#pragma once

#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    private:

      volatile bool stopped = false;
      unsigned short port = 27015;
      std::function<void(const std::string&)> logHandler = {};
      std::function<std::vector<char>(const std::vector<char>&)> exchangeHandler = {};

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      SocketServer() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(const std::function<void(const std::string&)>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(void(T::*method)(const std::string&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setExchangeHandler(const std::function<std::vector<char>(const std::vector<char>&)>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setExchangeHandler(std::vector<char>(T::*method)(const std::vector<char>&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void run();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void stop();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      ~SocketServer() = default;

    private:

      void log(const std::string& message);

  };

}

namespace exqudens {

  template<class T>
  void SocketServer::setLogHandler(void(T::*method)(const std::string&), void* object) {
    logHandler = std::bind(method, reinterpret_cast<T*>(object), std::placeholders::_1);
  }

  template<class T>
  void SocketServer::setExchangeHandler(std::vector<char>(T::*method)(const std::vector<char>&), void* object) {
    exchangeHandler = std::bind(method, reinterpret_cast<T*>(object), std::placeholders::_1);
  }

}
