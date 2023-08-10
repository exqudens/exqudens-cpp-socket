#include "exqudens/SocketServer.hpp"

#include <cstddef>
#include <cstring>
#include <string>
#include <stdexcept>

#include <winsock2.h>
#include <ws2tcpip.h>

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
      //size_t listenSocket;
      size_t tmpListenSocket;
      int lastError;
      int bindResult;
      int listenResult;
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

      listenSocket = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);

      if (listenSocket == INVALID_SOCKET) {
        lastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(listenSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'socket' success. listenSocket: '" + std::to_string(listenSocket) + "'");

      bindResult = bind(listenSocket, addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (bindResult != 0) {
        lastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        closesocket(listenSocket);
        errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      log("'bind' success.");

      listenResult = listen(listenSocket, SOMAXCONN);

      if (listenResult != 0) {
        lastError = WSAGetLastError();
        closesocket(listenSocket);
        errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'listen' success.");

      acceptedSocket = accept(listenSocket, nullptr, nullptr);

      if (acceptedSocket == INVALID_SOCKET) {
        if (listenSocket == INVALID_SOCKET) {
          return;
        }
        lastError = WSAGetLastError();
        closesocket(listenSocket);
        errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(acceptedSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      tmpListenSocket = listenSocket;
      listenSocket = INVALID_SOCKET;
      closesocket(tmpListenSocket);

      log("'accept' success. acceptedSocket: '" + std::to_string(acceptedSocket) + "'");
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  std::vector<char> SocketServer::receiveData(const int& bufferSize) {
    try {
      std::vector<char> buffer = std::vector<char>(bufferSize > 0 ? bufferSize : 1024);
      int recvResult = recv(acceptedSocket, buffer.data(), (int) buffer.size(), 0);

      if (recvResult < 0) {
        int lastError = WSAGetLastError();
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

  int SocketServer::sendData(const std::vector<char>& value, const int& bufferSize) {
    try {
      size_t internalBufferSize = bufferSize > 0 ? bufferSize : 1024;
      for (size_t i = 0; i < value.size(); i += internalBufferSize) {
        std::vector<char> outputBuffer = {};
        for (size_t j = 0; j < internalBufferSize && i + j < value.size(); j++) {
          outputBuffer.emplace_back(value.at(i + j));
        }

        int sendResult = send(acceptedSocket, outputBuffer.data(), (int) outputBuffer.size(), 0);

        if (sendResult < 0) {
          int lastError = WSAGetLastError();
          destroy();
          std::string errorMessage = "'send' failed with result: '";
          errorMessage += std::to_string(sendResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'send' success. bytes: '" + std::to_string(sendResult) + "'");
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketServer::destroy() {
    try {
      if (listenSocket != INVALID_SOCKET) {
        size_t tmpListenSocket = listenSocket;
        listenSocket = INVALID_SOCKET;

        int closeSocketResult = closesocket(tmpListenSocket);

        if (closeSocketResult < 0) {
          int lastError = WSAGetLastError();
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'closesocket' success. listenSocket: '" + std::to_string(tmpListenSocket) + "'");
      }
      if (acceptedSocket != INVALID_SOCKET) {
        size_t tmpAcceptedSocket = acceptedSocket;
        acceptedSocket = INVALID_SOCKET;

        int shutdownResult = shutdown(tmpAcceptedSocket, SD_SEND);

        if (shutdownResult < 0) {
          int lastError = WSAGetLastError();
          closesocket(tmpAcceptedSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'shutdown' success. acceptedSocket: '" + std::to_string(tmpAcceptedSocket) + "'");

        int closeSocketResult = closesocket(tmpAcceptedSocket);

        if (closeSocketResult < 0) {
          int lastError = WSAGetLastError();
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'closesocket' success. acceptedSocket: '" + std::to_string(tmpAcceptedSocket) + "'");
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
