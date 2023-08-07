#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <functional>

#include "exqudens/socket_export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketClient {

    private:

      std::string host = "localhost";
      unsigned short port = 27015;
      std::function<void(const std::string&)> logHandler = {};
      size_t connectSocket;
      int sendBufferSize = 1024;
      int receiveBufferSize = 1024;

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      SocketClient() = default;

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setHost(const std::string& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setPort(const unsigned short& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(const std::function<void(const std::string&)>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void setLogHandler(void(T::*method)(const std::string&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void connection();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      int sendData(const std::vector<char>& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      std::vector<char> receiveData();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      void disconnection();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      std::vector<char> exchange(const std::vector<char>& value);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      ~SocketClient() = default;

    private:

      void log(const std::string& message);

  };

}

namespace exqudens {

  template<class T>
  void SocketClient::setLogHandler(void(T::*method)(const std::string&), void* object) {
    logHandler = std::bind(method, reinterpret_cast<T*>(object), std::placeholders::_1);
  }

}
