#pragma once

#include "exqudens/socket/ISocketFactory.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT SocketFactory : public ISocketFactory {

    protected:

      std::function<void(
          const std::string&,
          const size_t&,
          const std::string&,
          const std::string&,
          const unsigned short&,
          const std::string&
      )> logFunction = {};
      bool initialized = false;

    public:

      EXQUDENS_SOCKET_INLINE
      void setLogFunction(
          const std::function<void(
              const std::string&,
              const size_t&,
              const std::string&,
              const std::string&,
              const unsigned short&,
              const std::string&
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
      ~SocketFactory() noexcept override = default;

    protected:

      EXQUDENS_SOCKET_INLINE
      virtual void log(
          const std::string& file,
          const size_t& line,
          const std::string& function,
          const std::string& id,
          const unsigned short& level,
          const std::string& message
      );

  };

}
