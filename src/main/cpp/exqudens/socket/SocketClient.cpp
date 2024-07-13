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

typedef int SOCKET;
#endif

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGER_ID "exqudens.SocketClient"
#define LOG_FATAL 1
#define LOG_ERROR 2
#define LOG_WARNING 3
#define LOG_INFO 4
#define LOG_DEBUG 5
#define LOG_VERBOSE 6

namespace exqudens {

  void SocketClient::setHost(const std::string& value) {
    host = value;
  }

  void SocketClient::init() {
    try {

      std::string errorMessage;
      int lastError;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      std::pair<size_t, std::string> socketResult = {SIZE_MAX, "-1"};
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

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'getaddrinfo' success.");

      for(addrinfo* i = addressInfo; i != nullptr; i = i->ai_next) {
        socketResult = openSocket(i->ai_family, i->ai_socktype, i->ai_protocol);

        if (socketResult.first == SIZE_MAX) {
          lastError = getLastError();
          errorMessage = "'socket' failed with result: '";
          errorMessage += socketResult.second;
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
        }

        transferSocket.store(socketResult.first);

        log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'socket' success. transferSocket: '" + std::to_string(transferSocket.load()) + "'");

        connectResult = connect((SOCKET) transferSocket.load(), i->ai_addr, (int) i->ai_addrlen);

        if (connectResult < 0) {
          closeSocket(transferSocket.load());
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

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'socket' success.");

      if (connectResult < 0) {
        errorMessage = "'connect' failed with result: '";
        errorMessage += std::to_string(connectResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'connect' success.");

    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketClient::destroy() {
    try {
      if (transferSocket.load() != SIZE_MAX) {
        size_t tmpTransferSocket = transferSocket.load();
        transferSocket.store(SIZE_MAX);

        int shutdownResult = shutdownSocket(tmpTransferSocket);

        if (shutdownResult < 0) {
          int lastError = getLastError();
          //closeSocket(tmpTransferSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'shutdown' warning " + errorMessage);
        }

        log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'shutdown' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'");

        int closeSocketResult = closeSocket(tmpTransferSocket);

        if (closeSocketResult < 0) {
          int lastError = getLastError();
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          //log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'close' warning " + errorMessage);
        }

        log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'closesocket' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'");
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::string SocketClient::getLoggerId() {
    try {
      return std::string(LOGGER_ID);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
#undef LOGGER_ID
#undef LOG_FATAL
#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_VERBOSE
