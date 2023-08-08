#include "exqudens/SocketServer.hpp"

#include <cstddef>
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

  void SocketServer::setReceiveHandler(const std::function<void(const std::vector<char>&)>& value) {
    receiveHandler = value;
  }

  void SocketServer::setSendHandler(const std::function<std::vector<char>()>& value) {
    sendHandler = value;
  }

  void SocketServer::setReceiveBufferSize(const int& value) {
    receiveBufferSize = value > 0 ? value : 1024;
  }

  void SocketServer::setSendBufferSize(const int& value) {
    sendBufferSize = value > 0 ? value : 1024;
  }

  void SocketServer::setStopped(const bool& value) {
    stopped = value;
  }

  void SocketServer::runOnce() {
    try {
      std::string errorMessage;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      //size_t listenSocket;
      int wsaLastError;
      int bindResult;
      int listenResult;
      size_t acceptedSocket;
      std::vector<char> inputBuffer;
      int recvResult;
      std::vector<char> outputBuffer;
      int sendResult;
      int shutdownResult;

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
        wsaLastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(listenSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'socket' success. listenSocket: '" + std::to_string(listenSocket) + "'");

      bindResult = bind(listenSocket, addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (bindResult != NO_ERROR) {
        wsaLastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        closesocket(listenSocket);
        errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      log("'bind' success.");

      listenResult = listen(listenSocket, SOMAXCONN);

      if (listenResult != NO_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(listenSocket);
        errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'listen' success.");

      acceptedSocket = accept(listenSocket, nullptr, nullptr);

      if (acceptedSocket == INVALID_SOCKET) {
        if (stopped) {
          return;
        }
        wsaLastError = WSAGetLastError();
        closesocket(listenSocket);
        errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(acceptedSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      closesocket(listenSocket);

      log("'accept' success. acceptedSocket: '" + std::to_string(acceptedSocket) + "'");

      do {
        inputBuffer = std::vector<char>(receiveBufferSize);
        recvResult = recv(acceptedSocket, inputBuffer.data(), (int) inputBuffer.size(), 0);

        if (recvResult < 0 || recvResult > (int) inputBuffer.size()) {
          wsaLastError = WSAGetLastError();
          closesocket(acceptedSocket);
          errorMessage = "'recv' failed with result: '";
          errorMessage += std::to_string(recvResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }  else if (recvResult == 0) {
          log("'recv' success. connection closing");
        } else {
          outputBuffer = std::vector<char>(inputBuffer.begin(), inputBuffer.begin() + recvResult);
          receiveHandler(outputBuffer);
          log("'recv' success. bytes: '" + std::to_string(recvResult) + "'");
        }
      } while (recvResult > 0);

      do {
        outputBuffer = sendHandler();

        if (outputBuffer.size() > (size_t) sendBufferSize) {
          closesocket(acceptedSocket);
          errorMessage = "'send' failed output buffer size: '";
          errorMessage += std::to_string(outputBuffer.size());
          errorMessage += "' greater than max send buffer size: '";
          errorMessage += std::to_string(sendBufferSize);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        if (outputBuffer.empty()) {
          break;
        }

        sendResult = send(acceptedSocket, outputBuffer.data(), (int) outputBuffer.size(), 0);

        if (sendResult == SOCKET_ERROR) {
          wsaLastError = WSAGetLastError();
          closesocket(acceptedSocket);
          errorMessage = "'send' failed with result: '";
          errorMessage += std::to_string(sendResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'send' success. bytes: '" + std::to_string(sendResult) + "'");
      } while (sendResult > 0);

      shutdownResult = shutdown(acceptedSocket, SD_SEND);

      if (shutdownResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(acceptedSocket);
        errorMessage = "'shutdown' failed with result: '";
        errorMessage += std::to_string(shutdownResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'shutdown' success.");

      closesocket(acceptedSocket);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketServer::stop() {
    try {
      if (listenSocket != INVALID_SOCKET) {
        log("'closesocket' listenSocket: '" + std::to_string(listenSocket) + "'");
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        stopped = true;
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
