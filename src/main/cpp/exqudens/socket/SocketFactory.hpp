#pragma once

#include "exqudens/socket/ISocketFactory.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketFactory : public ISocketFactory {

    protected:

      std::function<void(
          const std::string&,
          const unsigned short&,
          const std::string&,
          const std::string&,
          const size_t&
      )> logFunction = {};
      bool initialized = false;

    public:

      EXQUDENS_SOCKET_INLINE
      void setLogFunction(
          const std::function<void(
              const std::string&,
              const unsigned short&,
              const std::string&,
              const std::string&,
              const size_t&
          )>& value
      ) override;

      EXQUDENS_SOCKET_INLINE
      bool isInitialized() override;

      EXQUDENS_SOCKET_INLINE
      void init() override;

      EXQUDENS_SOCKET_INLINE
      std::shared_ptr<ISocket> createSharedServer() override;

      EXQUDENS_SOCKET_INLINE
      std::unique_ptr<ISocket> createUniqueServer() override;

      EXQUDENS_SOCKET_INLINE
      std::shared_ptr<ISocket> createSharedClient() override;

      EXQUDENS_SOCKET_INLINE
      std::unique_ptr<ISocket> createUniqueClient() override;

      EXQUDENS_SOCKET_INLINE
      void destroy() override;

      EXQUDENS_SOCKET_INLINE
      ~SocketFactory() override = default;

    protected:

      EXQUDENS_SOCKET_INLINE
      virtual void log(
          const std::string& message,
          const unsigned short& level,
          const std::string& function,
          const std::string& file,
          const size_t& line
      );

  };

}
