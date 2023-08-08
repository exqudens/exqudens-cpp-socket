#pragma once

#include <string>
#include <functional>

#include "exqudens/socket_export.hpp"

#include "exqudens/SocketServer.hpp"
#include "exqudens/SocketClient.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT Sockets {

    private:

      inline static std::function<void(const std::string&)> logHandler = {};
      inline static bool initialized = false;

    public:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      static void setLogHandler(const std::function<void(const std::string&)>& value);

      template<class T>
      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      inline void setLogHandler(void(T::*method)(const std::string&), void* object);

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      static void init();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      static SocketServer createServer();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      static SocketClient createClient();

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      static void destroy();

    private:

      EXQUDENS_SOCKET_FUNCTION_ATTRIBUTES
      static void log(const std::string& message);

  };

}

namespace exqudens {

  template<class T>
  void Sockets::setLogHandler(void(T::*method)(const std::string&), void* object) {
    logHandler = std::bind(method, reinterpret_cast<T*>(object), std::placeholders::_1);
  }

}
