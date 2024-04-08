#include <cstring>
#include <stdexcept>
#include <filesystem>

#include "exqudens/socket/SocketClient.hpp"

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define LOG_TRACE 5

namespace exqudens {

  void SocketClient::setHost(const std::string& value) {
    host = value;
  }

  void SocketClient::init() {
    try {

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      std::string errorMessage;
      int lastError;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      //size_t transferSocket;
      int connectResult;

      std::memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;

      getAddrInfoResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addressInfo);

      if (getAddrInfoResult != 0) {
        errorMessage = "'getaddrinfo' failed with result: '";
        errorMessage += std::to_string(getAddrInfoResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'getaddrinfo' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      for(addrinfo* i = addressInfo; i != nullptr; i = i->ai_next) {
        transferSocket.store(socket(i->ai_family, i->ai_socktype, i->ai_protocol));

        if (transferSocket.load() == INVALID_SOCKET) {
          lastError = WSAGetLastError();
          errorMessage = "'socket' failed with result: '";
          errorMessage += std::to_string(transferSocket.load());
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
        }

        log("'socket' success. transferSocket: '" + std::to_string(transferSocket.load()) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

        connectResult = connect(transferSocket.load(), i->ai_addr, (int) i->ai_addrlen);

        if (connectResult < 0) {
          closesocket(transferSocket.load());
          transferSocket.store(INVALID_SOCKET);
          continue;
        }

        break;
      }

      freeaddrinfo(addressInfo);

      if (transferSocket.load() == INVALID_SOCKET) {
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(transferSocket.load());
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'socket' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      if (connectResult < 0) {
        errorMessage = "'connect' failed with result: '";
        errorMessage += std::to_string(connectResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'connect' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

#else

      std::string errorMessage;
      int lastError;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      int socketResult;
      //size_t transferSocket;
      int connectResult;

      std::memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;

      getAddrInfoResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addressInfo);

      if (getAddrInfoResult != 0) {
        errorMessage = "'getaddrinfo' failed with result: '";
        errorMessage += std::to_string(getAddrInfoResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'getaddrinfo' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      for(addrinfo* i = addressInfo; i != nullptr; i = i->ai_next) {
        socketResult = socket(i->ai_family, i->ai_socktype, i->ai_protocol);

        if (socketResult < 0) {
          lastError = errno;
          errorMessage = "'socket' failed with result: '";
          errorMessage += std::to_string(socketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
        }

        transferSocket.store(socketResult);

        log("'socket' success. transferSocket: '" + std::to_string(transferSocket.load()) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

        connectResult = connect((int) transferSocket.load(), i->ai_addr, (int) i->ai_addrlen);

        if (connectResult < 0) {
          close((int) transferSocket.load());
          transferSocket.store(SIZE_MAX);
          continue;
        }

        break;
      }

      freeaddrinfo(addressInfo);

      if (transferSocket.load() == SIZE_MAX) {
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(transferSocket.load());
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'socket' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      if (connectResult < 0) {
        errorMessage = "'connect' failed with result: '";
        errorMessage += std::to_string(connectResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'connect' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

#endif

    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketClient::destroy() {
    try {

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      if (transferSocket.load() != INVALID_SOCKET) {
        size_t tmpTransferSocket = transferSocket.load();
        transferSocket.store(INVALID_SOCKET);

        int shutdownResult = shutdown(tmpTransferSocket, SD_BOTH);

        if (shutdownResult < 0) {
          int lastError = WSAGetLastError();
          //closesocket(tmpTransferSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          log("'shutdown' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'shutdown' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

        int closeSocketResult = closesocket(tmpTransferSocket);

        if (closeSocketResult < 0) {
          int lastError = WSAGetLastError();
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          //log("'closesocket' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'closesocket' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }

#else

      if (transferSocket.load() != SIZE_MAX) {
        size_t tmpTransferSocket = transferSocket.load();
        transferSocket.store(SIZE_MAX);

        int shutdownResult = shutdown((int) tmpTransferSocket, SHUT_WR);

        if (shutdownResult < 0) {
          int lastError = errno;
          //close((int) tmpTransferSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          log("'shutdown' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'shutdown' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

        int closeSocketResult = close((int) tmpTransferSocket);

        if (closeSocketResult < 0) {
          int lastError = errno;
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          ////log("'close' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'closesocket' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }

#endif

    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
#undef LOG_FATAL
#undef LOG_ERROR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_TRACE
