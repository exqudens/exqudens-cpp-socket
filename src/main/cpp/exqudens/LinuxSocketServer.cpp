#include "exqudens/SocketServer.hpp"

#include <cstring>
#include <string>
#include <stdexcept>

#include <netdb.h>
#include <unistd.h>

namespace exqudens {

  void SocketServer::setPort(const unsigned short& value) {
    port = value;
  }

  void SocketServer::setLogHandler(const std::function<void(const std::string&)>& value) {
    logHandler = value;
  }

  void SocketServer::init() {
    try {
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
      //size_t acceptedSocket;
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
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'getaddrinfo' success.");

      socketResult = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);

      if (socketResult < 0) {
        lastError = errno;
        freeaddrinfo(addressInfo);
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(socketResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      listenSocket = socketResult;

      log("'socket' success. listenSocket: '" + std::to_string(listenSocket) + "'");

      bindResult = bind((int) listenSocket, addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (bindResult != 0) {
        lastError = errno;
        freeaddrinfo(addressInfo);
        close((int) listenSocket);
        errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      log("'bind' success.");

      listenResult = listen((int) listenSocket, SOMAXCONN);

      if (listenResult != 0) {
        lastError = errno;
        close((int) listenSocket);
        errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'listen' success.");

      acceptResult = accept((int) listenSocket, nullptr, nullptr);

      if (acceptResult < 0) {
        if (listenSocket == SIZE_MAX) {
          return;
        }
        lastError = errno;
        close((int) listenSocket);
        errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(acceptResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      acceptedSocket = acceptResult;

      tmpListenSocket = listenSocket;
      listenSocket = SIZE_MAX;
      close((int) tmpListenSocket);

      log("'accept' success. acceptedSocket: '" + std::to_string(acceptedSocket) + "'");
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  std::vector<char> SocketServer::receiveData(const int& bufferSize) {
    try {
      std::vector<char> buffer = std::vector<char>(bufferSize > 0 ? bufferSize : 1024);
      ssize_t recvResult = recv((int) acceptedSocket, buffer.data(), (int) buffer.size(), 0);

      if (recvResult < 0) {
        int lastError = errno;
        destroy();
        std::string errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }  else if (recvResult == 0) {
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'");
      } else {
        buffer = std::vector(buffer.begin(), buffer.begin() + recvResult);
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'");
      }

      return buffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  int SocketServer::sendData(const std::vector<char>& buffer) {
    try {
      if (buffer.size() > INT_MAX) {
        std::string errorMessage = "'buffer' size: '";
        errorMessage += std::to_string(buffer.size());
        errorMessage += "' is greater than max buffer size: '";
        errorMessage += std::to_string(INT_MAX);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      ssize_t sendResult = send((int) acceptedSocket, buffer.data(), (int) buffer.size(), 0);

      if (sendResult < 0) {
        int lastError = errno;
        destroy();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'send' success. bytes: '" + std::to_string(sendResult) + "'");

      return (int) sendResult;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketServer::destroy() {
    try {
      if (acceptedSocket != SIZE_MAX) {
        size_t tmpAcceptedSocket = acceptedSocket;
        acceptedSocket = SIZE_MAX;

        int shutdownResult = shutdown((int) tmpAcceptedSocket, SHUT_RDWR);

        if (shutdownResult < 0) {
          int lastError = errno;
          //close((int) tmpAcceptedSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
          log("'shutdown' warning " + errorMessage);
        }

        log("'shutdown' success. acceptedSocket: '" + std::to_string(tmpAcceptedSocket) + "'");

        int closeSocketResult = close((int) tmpAcceptedSocket);

        if (closeSocketResult < 0) {
          int lastError = errno;
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'closesocket' success. acceptedSocket: '" + std::to_string(tmpAcceptedSocket) + "'");
      }
      if (listenSocket != SIZE_MAX) {
        size_t tmpListenSocket = listenSocket;
        listenSocket = SIZE_MAX;

        int shutdownResult = shutdown((int) tmpListenSocket, SHUT_RDWR);

        if (shutdownResult < 0) {
          int lastError = errno;
          //close((int) tmpListenSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          //throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
          log("'shutdown' warning " + errorMessage);
        }

        log("'shutdown' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'");

        int closeSocketResult = close((int) tmpListenSocket);

        if (closeSocketResult < 0) {
          int lastError = errno;
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'closesocket' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'");
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketServer::log(const std::string& message) {
    try {
      if (logHandler) {
        logHandler(message);
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

}
