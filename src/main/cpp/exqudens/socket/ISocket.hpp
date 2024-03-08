#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <functional>

#include "exqudens/socket/export.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT ISocket {

    public:

      EXQUDENS_SOCKET_INLINE
      virtual void setPort(const unsigned short& value) = 0;

      EXQUDENS_SOCKET_INLINE
      virtual void setHost(const std::string& value) = 0;

      EXQUDENS_SOCKET_INLINE
      virtual void setLogFunction(
          const std::function<void(
              const std::string&,
              const unsigned short&,
              const std::string&,
              const std::string&,
              const size_t&
          )>& value
      ) = 0;

      EXQUDENS_SOCKET_INLINE
      virtual void init() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual size_t sendData(const std::vector<char>& buffer) = 0;

      EXQUDENS_SOCKET_INLINE
      virtual std::vector<char> receiveData(const size_t& bufferSize) = 0;

      EXQUDENS_SOCKET_INLINE
      virtual std::vector<char> receiveData() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual void destroy() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual ~ISocket() = default;

  };

}
