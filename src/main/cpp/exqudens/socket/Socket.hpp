#pragma once

#include <climits>
#include <atomic>
#include <utility>

#include "exqudens/socket/ISocket.hpp"

namespace exqudens {

  class EXQUDENS_SOCKET_EXPORT Socket : public ISocket {

    protected:

      unsigned short port = 27015;
      std::string host = "localhost";
      std::function<void(
          const std::string&,
          const size_t&,
          const std::string&,
          const std::string&,
          const unsigned short&,
          const std::string&
      )> logFunction = {};

      std::atomic<size_t> listenSocket = SIZE_MAX;
      std::atomic<size_t> transferSocket = SIZE_MAX;

    public:

      EXQUDENS_SOCKET_INLINE
      void setPort(const unsigned short& value) override;

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
      size_t sendData(const std::vector<char>& buffer) override;

      EXQUDENS_SOCKET_INLINE
      std::vector<char> receiveData(const size_t& bufferSize) override;

      EXQUDENS_SOCKET_INLINE
      std::vector<char> receiveData() override;

      EXQUDENS_SOCKET_INLINE
      ~Socket() noexcept override = default;

    protected:

      EXQUDENS_SOCKET_INLINE
      virtual int getLastError();

      EXQUDENS_SOCKET_INLINE
      virtual std::pair<size_t, std::string> openSocket(const int& family, const int& type, const int& protocol);

      EXQUDENS_SOCKET_INLINE
      virtual std::pair<size_t, std::string> acceptSocket(const size_t& value);

      EXQUDENS_SOCKET_INLINE
      virtual int closeSocket(const size_t& value);

      EXQUDENS_SOCKET_INLINE
      virtual int shutdownSocket(const size_t& value);

      EXQUDENS_SOCKET_INLINE
      virtual void log(
          const std::string& file,
          const size_t& line,
          const std::string& function,
          const std::string& id,
          const unsigned short& level,
          const std::string& message
      );

      EXQUDENS_SOCKET_INLINE
      virtual std::string getLoggerId() = 0;

  };

}
