#include "exqudens/SocketServer.hpp"

#include <cstddef>
#include <string>
#include <stdexcept>

#include <winsock2.h>
#include <ws2tcpip.h>

namespace exqudens {

  SocketServer& SocketServer::setPort(const unsigned short& value) {
    port = value;
    return *this;
  }

  SocketServer& SocketServer::setReceiveFunction(const std::function<std::vector<char>(const std::vector<char>&)>& value) {
    receiveFunction = value;
    return *this;
  }

  void SocketServer::start() {
    try {
      WSADATA wsaData;
      int result = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (result != 0) {
        std::string errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      addrinfo hints = {};
      std::memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_flags = AI_PASSIVE;
      addrinfo* addressInfo = nullptr;
      int wsaLastError = 0;
      result = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &addressInfo);

      if (result != 0) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'getaddrinfo' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      size_t listenSocket = socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);

      if (listenSocket == INVALID_SOCKET) {
        wsaLastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        WSACleanup();
        std::string errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(listenSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      result = bind(listenSocket, addressInfo->ai_addr, (int) addressInfo->ai_addrlen);

      if (result == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        freeaddrinfo(addressInfo);
        WSACleanup();
        std::string errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      freeaddrinfo(addressInfo);

      result = listen(listenSocket, SOMAXCONN);
      if (result == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      size_t clientSocket = accept(listenSocket, nullptr, nullptr);

      if (clientSocket == INVALID_SOCKET) {
        wsaLastError = WSAGetLastError();
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(clientSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      closesocket(listenSocket);
      int bufferSize = 1024;

      do {
        std::vector<char> buffer(bufferSize);
        result = recv(clientSocket, buffer.data(), bufferSize, 0);

        if (result < 0 || result > bufferSize) {
          wsaLastError = WSAGetLastError();
          closesocket(clientSocket);
          WSACleanup();
          std::string errorMessage = "'recv' failed with result: '";
          errorMessage += std::to_string(result);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        } else if (result > 0) {
          std::vector<char> bufferTmp(buffer.begin(), buffer.begin() + result);
          std::vector<char> sendBuffer = receiveFunction(bufferTmp);
          int sendResult = send(clientSocket, sendBuffer.data(), (int) sendBuffer.size(), 0);

          if (sendResult == SOCKET_ERROR) {
            wsaLastError = WSAGetLastError();
            closesocket(clientSocket);
            WSACleanup();
            std::string errorMessage = "'send' failed with result: '";
            errorMessage += std::to_string(sendResult);
            errorMessage += "' error: '";
            errorMessage += std::to_string(wsaLastError);
            errorMessage += "'";
            throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
          }
        }
      } while (result > 0);

      result = shutdown(clientSocket, SD_SEND);

      if (result == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        std::string errorMessage = "'shutdown' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      closesocket(clientSocket);
      WSACleanup();

    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketServer::stop() {
    try {
      //
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

}
