#include <cstring>
#include <stdexcept>
#include <filesystem>

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;
#endif

#include "exqudens/socket/SocketServer.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGER_ID "exqudens.SocketServer"
#define LOG_FATAL 1
#define LOG_ERROR 2
#define LOG_WARNING 3
#define LOG_INFO 4
#define LOG_DEBUG 5
#define LOG_VERBOSE 6

namespace exqudens {

  void SocketServer::setHost(const std::string& value) {
    throw std::runtime_error(CALL_INFO + ": Not applicable!!!");
  }

  void SocketServer::init() {
    try {
      std::string errorMessage;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      std::pair<size_t, std::string> socketResult = {SIZE_MAX, "-1"};
      size_t tmpListenSocket;
      int lastError;
      int bindResult;
      int listenResult;
      std::pair<size_t, std::string> acceptResult = {SIZE_MAX, "-1"};

      std::memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_flags = AI_PASSIVE;

      getAddrInfoResult = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &addressInfo);

      if (getAddrInfoResult != 0) {
        errorMessage = "'getaddrinfo' failed with result: '";
        errorMessage += std::to_string(getAddrInfoResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'getaddrinfo' success.");

      socketResult = openSocket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);

      if (socketResult.first == SIZE_MAX) {
        lastError = getLastError();
        errorMessage = "'socket' failed with result: '";
        errorMessage += socketResult.second;
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      listenSocket.store(socketResult.first);

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'socket' success. listenSocket: '" + std::to_string(listenSocket.load()) + "'");

      bindResult = bind((SOCKET) listenSocket.load(), addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (bindResult != 0) {
        lastError = getLastError();
        freeaddrinfo(addressInfo);
        closeSocket(listenSocket.load());
        errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'bind' success.");

      listenResult = listen((SOCKET) listenSocket.load(), SOMAXCONN);

      if (listenResult != 0) {
        lastError = getLastError();
        closeSocket(listenSocket.load());
        errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'listen' success.");

      acceptResult = acceptSocket((SOCKET) listenSocket.load());

      if (acceptResult.first == SIZE_MAX) {
        if (listenSocket.load() == SIZE_MAX) {
          return;
        }
        lastError = getLastError();
        closeSocket(listenSocket.load());
        errorMessage = "'accept' failed with result: '";
        errorMessage += acceptResult.second;
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      transferSocket.store(acceptResult.first);

      tmpListenSocket = listenSocket.load();
      listenSocket.store(SIZE_MAX);
      closeSocket(tmpListenSocket);

      log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'accept' success. transferSocket: '" + std::to_string(transferSocket.load()) + "'");
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketServer::destroy() {
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
      if (listenSocket.load() != SIZE_MAX) {
        size_t tmpListenSocket = listenSocket.load();
        listenSocket.store(SIZE_MAX);

        int shutdownResult = shutdownSocket(tmpListenSocket);

        if (shutdownResult < 0) {
          int lastError = errno;
          //closeSocket(tmpListenSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'shutdown' warning " + errorMessage);
        }

        log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'shutdown' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'");

        int closeSocketResult = closeSocket(tmpListenSocket);

        if (closeSocketResult < 0) {
          int lastError = errno;
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          //log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'close' warning " + errorMessage);
        }

        log(__FILE__, __LINE__, __FUNCTION__, getLoggerId(), LOG_INFO, "'closesocket' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'");
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::string SocketServer::getLoggerId() {
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
