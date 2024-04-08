#pragma once

#include <climits>
#include <atomic>

#include "exqudens/socket/ISocket.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT Socket : public ISocket {

    protected:

      unsigned short port = 27015;
      std::string host = "localhost";
      std::function<void(
          const std::string&,
          const unsigned short&,
          const std::string&,
          const std::string&,
          const size_t&
      )> logFunction = {};

      std::atomic<size_t> listenSocket = SIZE_MAX;
      std::atomic<size_t> transferSocket = SIZE_MAX;

    public:

      EXQUDENS_SOCKET_INLINE
      void setPort(const unsigned short& value) override;

      EXQUDENS_SOCKET_INLINE
      void setHost(const std::string& value) override;

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
      void init() override;

      EXQUDENS_SOCKET_INLINE
      size_t sendData(const std::vector<char>& buffer) override;

      EXQUDENS_SOCKET_INLINE
      std::vector<char> receiveData(const size_t& bufferSize) override;

      EXQUDENS_SOCKET_INLINE
      std::vector<char> receiveData() override;

      EXQUDENS_SOCKET_INLINE
      void destroy() override;

      EXQUDENS_SOCKET_INLINE
      ~Socket() override = default;

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
