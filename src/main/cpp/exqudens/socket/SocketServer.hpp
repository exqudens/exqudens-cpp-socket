#pragma once

#include "exqudens/socket/Socket.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketServer : public Socket {

    public:

      EXQUDENS_SOCKET_INLINE
      SocketServer(const std::string& loggerId);

      EXQUDENS_SOCKET_INLINE
      SocketServer();

      EXQUDENS_SOCKET_INLINE
      void setHost(const std::string& value) override;

      EXQUDENS_SOCKET_INLINE
      void init() override;

      EXQUDENS_SOCKET_INLINE
      void destroy() override;

      EXQUDENS_SOCKET_INLINE
      ~SocketServer() noexcept override = default;

  };

}
