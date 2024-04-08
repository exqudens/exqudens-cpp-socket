#include <cstring>
#include <stdexcept>
#include <filesystem>

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <unistd.h>
#endif

#include "exqudens/socket/SocketServer.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define LOG_TRACE 5

namespace exqudens {

  void SocketServer::init() {
    try {

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      std::string errorMessage;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      //size_t listenSocket;
      size_t tmpListenSocket;
      int lastError;
      int bindResult;
      int listenResult;
      //size_t transferSocket;
      std::vector<char> buffer;

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

      log("'getaddrinfo' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      listenSocket.store((size_t) socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol));

      if (listenSocket.load() == INVALID_SOCKET) {
        lastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(listenSocket.load());
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'socket' success. listenSocket: '" + std::to_string(listenSocket.load()) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      bindResult = bind(listenSocket.load(), addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (bindResult != 0) {
        lastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        closesocket(listenSocket.load());
        errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      log("'bind' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      listenResult = listen(listenSocket.load(), SOMAXCONN);

      if (listenResult != 0) {
        lastError = WSAGetLastError();
        closesocket(listenSocket.load());
        errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'listen' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      transferSocket.store(accept(listenSocket.load(), nullptr, nullptr));

      if (transferSocket.load() == INVALID_SOCKET) {
        if (listenSocket.load() == INVALID_SOCKET) {
          return;
        }
        lastError = WSAGetLastError();
        closesocket(listenSocket.load());
        errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(transferSocket.load());
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      tmpListenSocket = listenSocket.load();
      listenSocket.store(INVALID_SOCKET);
      closesocket(tmpListenSocket);

      log("'accept' success. transferSocket: '" + std::to_string(transferSocket.load()) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

#else

      std::string errorMessage;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      int socketResult;
      //size_t listenSocket;
      size_t tmpListenSocket;
      int lastError;
      int bindResult;
      int listenResult;
      int acceptResult;
      //size_t transferSocket;
      std::vector<char> buffer;

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

      log("'getaddrinfo' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      socketResult = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);

      if (socketResult < 0) {
        lastError = errno;
        freeaddrinfo(addressInfo);
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(socketResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      listenSocket.store(socketResult);

      log("'socket' success. listenSocket: '" + std::to_string(listenSocket.load()) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      bindResult = bind((int) listenSocket.load(), addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (bindResult != 0) {
        lastError = errno;
        freeaddrinfo(addressInfo);
        close((int) listenSocket.load());
        errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      log("'bind' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      listenResult = listen((int) listenSocket.load(), SOMAXCONN);

      if (listenResult != 0) {
        lastError = errno;
        close((int) listenSocket.load());
        errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      log("'listen' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

      acceptResult = accept((int) listenSocket.load(), nullptr, nullptr);

      if (acceptResult < 0) {
        if (listenSocket.load() == SIZE_MAX) {
          return;
        }
        lastError = errno;
        close((int) listenSocket.load());
        errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(acceptResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      transferSocket.store(acceptResult);

      tmpListenSocket = listenSocket.load();
      listenSocket.store(SIZE_MAX);
      close((int) tmpListenSocket);

      log("'accept' success. transferSocket: '" + std::to_string(transferSocket.load()) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

#endif

    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketServer::destroy() {
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
        }

        log("'closesocket' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }
      if (listenSocket.load() != INVALID_SOCKET) {
        size_t tmpListenSocket = listenSocket.load();
        listenSocket.store(INVALID_SOCKET);

        int shutdownResult = shutdown(tmpListenSocket, SD_BOTH);

        if (shutdownResult < 0) {
          int lastError = WSAGetLastError();
          //closesocket(tmpListenSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          log("'shutdown' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'shutdown' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

        int closeSocketResult = closesocket(tmpListenSocket);

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

        log("'closesocket' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }

#else

      if (transferSocket.load() != SIZE_MAX) {
        size_t tmpTransferSocket = transferSocket.load();
        transferSocket.store(SIZE_MAX);

        int shutdownResult = shutdown((int) tmpTransferSocket, SHUT_RDWR);

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
          //log("'close' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'closesocket' success. transferSocket: '" + std::to_string(tmpTransferSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }
      if (listenSocket.load() != SIZE_MAX) {
        size_t tmpListenSocket = listenSocket.load();
        listenSocket.store(SIZE_MAX);

        int shutdownResult = shutdown((int) tmpListenSocket, SHUT_RDWR);

        if (shutdownResult < 0) {
          int lastError = errno;
          //close((int) tmpListenSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          log("'shutdown' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'shutdown' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

        int closeSocketResult = close((int) tmpListenSocket);

        if (closeSocketResult < 0) {
          int lastError = errno;
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
          //log("'close' warning " + errorMessage, LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
        }

        log("'closesocket' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
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
