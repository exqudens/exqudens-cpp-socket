#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include <functional>

#include "exqudens/socket/SocketInterface.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT Sockets {

    private:

      inline static std::function<void(const std::string&, const unsigned short&, const std::string&, const std::string&, const size_t&)> logFunction = {};
      inline static bool initialized = false;

    public:

      EXQUDENS_SOCKET_INLINE
      static void setLogFunction(const std::function<void(const std::string&, const unsigned short&, const std::string&, const std::string&, const size_t&)>& value);

      EXQUDENS_SOCKET_INLINE
      static void init();

      EXQUDENS_SOCKET_INLINE
      static std::shared_ptr<SocketInterface> createServer();

      EXQUDENS_SOCKET_INLINE
      static std::shared_ptr<SocketInterface> createClient();

      EXQUDENS_SOCKET_INLINE
      static void destroy();

    private:

      EXQUDENS_SOCKET_INLINE
      static void log(const std::string& message, const unsigned short& level, const std::string& function, const std::string& file, const size_t& line);

  };

}
