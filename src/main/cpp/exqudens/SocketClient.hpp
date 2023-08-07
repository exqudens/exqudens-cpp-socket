#pragma once

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
