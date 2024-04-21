#pragma once

#include "exqudens/socket/Socket.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketClient : public Socket {

    protected:

      std::string host = "localhost";

    public:

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
