#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include <functional>

#include "exqudens/socket/ISocket.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT ISocketFactory {

    public:

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
      virtual bool isInitialized() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual void init() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual std::shared_ptr<ISocket> createSharedServer() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual std::unique_ptr<ISocket> createUniqueServer() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual std::shared_ptr<ISocket> createSharedClient() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual std::unique_ptr<ISocket> createUniqueClient() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual void destroy() = 0;

      EXQUDENS_SOCKET_INLINE
      virtual ~ISocketFactory() = default;

  };

}
