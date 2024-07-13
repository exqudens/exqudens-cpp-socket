#pragma once

#include "exqudens/socket/Socket.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketClient : public Socket {

    public:

      EXQUDENS_SOCKET_INLINE
      SocketClient(const std::string& loggerId);

      EXQUDENS_SOCKET_INLINE
      SocketClient();

      EXQUDENS_SOCKET_INLINE
      void setHost(const std::string& value) override;

      EXQUDENS_SOCKET_INLINE
      void init() override;

      EXQUDENS_SOCKET_INLINE
      void destroy() override;

      EXQUDENS_SOCKET_INLINE
      ~SocketClient() noexcept override = default;

  };

}
