#pragma once

#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    private:

      unsigned short port = 27015;
      std::function<void(const std::string&)> logHandler = {};
      std::function<void(const std::vector<char>&)> receiveHandler = {};
      std::function<std::vector<char>()> sendHandler = {};

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      SocketServer() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(const std::function<void(const std::string&)>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(void(T::*method)(const std::string&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setReceiveHandler(const std::function<void(const std::vector<char>&)>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setReceiveHandler(void(T::* method)(const std::vector<char>&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setSendHandler(const std::function<std::vector<char>()>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setSendHandler(std::vector<char>(T::* method)(), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void runOnce();

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
  void SocketServer::setReceiveHandler(void(T::* method)(const std::vector<char>&), void* object) {
    receiveHandler = std::bind(method, reinterpret_cast<T*>(object), std::placeholders::_1);
  }

  template<class T>
  void SocketServer::setSendHandler(std::vector<char>(T::* method)(), void* object) {
    sendHandler = std::bind(method, reinterpret_cast<T*>(object));
  }

}
