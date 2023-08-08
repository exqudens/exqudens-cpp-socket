#pragma once

#include <cstddef>
#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer {

    friend class Sockets;

    private:

      unsigned short port = 27015;
      std::function<void(const std::string&)> logHandler = {};
      std::function<void(const std::vector<char>&)> receiveHandler = {};
      std::function<std::vector<char>()> sendHandler = {};
      int receiveBufferSize = 1024;
      int sendBufferSize = 1024;
      bool stopped = false;

      size_t listenSocket = ~0;

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(const std::function<void(const std::string&)>& value);

      template<class T>
      inline void setLogHandler(void(T::*method)(const std::string&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setReceiveHandler(const std::function<void(const std::vector<char>&)>& value);

      template<class T>
      inline void setReceiveHandler(void(T::* method)(const std::vector<char>&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setSendHandler(const std::function<std::vector<char>()>& value);

      template<class T>
      inline void setSendHandler(std::vector<char>(T::* method)(), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setReceiveBufferSize(const int& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setSendBufferSize(const int& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setStopped(const bool& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void runOnce();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void stop();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      ~SocketServer() = default;

    private:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      SocketServer() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
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
