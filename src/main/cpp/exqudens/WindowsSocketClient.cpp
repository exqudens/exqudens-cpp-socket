#include "exqudens/SocketClient.hpp"

#include <cstddef>
#include <string>
#include <stdexcept>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>

namespace exqudens {

  void SocketClient::setHost(const std::string& value) {
    host = value;
  }

  void SocketClient::setPort(const unsigned short& value) {
    port = value;
  }

  void SocketClient::setLogHandler(const std::function<void(const std::string&)>& value) {
    logHandler = value;
  }

  std::vector<char> SocketClient::exchange(const std::vector<char>& value) {
    try {
      WSADATA wsaData;
      int wsaDataResult;
      std::string errorMessage;
      int wsaLastError;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      size_t connectSocket;
      int connectResult;
      int sendResult;
      int shutdownResult;
      std::vector<char> inputBuffer;
      int recvResult;
      std::vector<char> outputBuffer;

      wsaDataResult = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (wsaDataResult != 0) {
        errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(wsaDataResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'WSAStartup' success.");

      getAddrInfoResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addressInfo);

      if (getAddrInfoResult != 0) {
        errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(wsaDataResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      for(addrinfo* i = addressInfo; i != nullptr; i = i->ai_next) {
        connectSocket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);

        if (connectSocket == INVALID_SOCKET) {
          wsaLastError = WSAGetLastError();
          WSACleanup();
          errorMessage = "'socket' failed with result: '";
          errorMessage += std::to_string(connectSocket);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'socket' success. connectSocket: '" + std::to_string(connectSocket) + "'");

        connectResult = connect(connectSocket, i->ai_addr, (int) i->ai_addrlen);

        if (connectResult == SOCKET_ERROR) {
          closesocket(connectSocket);
          connectSocket = INVALID_SOCKET;
          continue;
        }

        break;
      }

      freeaddrinfo(addressInfo);

      if (connectSocket == INVALID_SOCKET) {
        WSACleanup();
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(connectSocket);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'socket' success.");

      if (connectResult == SOCKET_ERROR) {
        WSACleanup();
        errorMessage = "'connect' failed with result: '";
        errorMessage += std::to_string(connectResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'connect' success.");

      sendResult = send(connectSocket, value.data(), (int) value.size(), 0);

      if (sendResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'send' success. bytes: '" + std::to_string(sendResult) + "'");

      shutdownResult = shutdown(connectSocket, SD_SEND);

      if (shutdownResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        errorMessage = "'shutdown' failed with result: '";
        errorMessage += std::to_string(shutdownResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'shutdown' success.");

      do {
        inputBuffer = std::vector<char>(1024);
        recvResult = recv(connectSocket, inputBuffer.data(), (int) inputBuffer.size(), 0);

        if (recvResult < 0) {
          wsaLastError = WSAGetLastError();
          closesocket(connectSocket);
          WSACleanup();
          errorMessage = "'recv' failed with result: '";
          errorMessage += std::to_string(recvResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        } else if (recvResult == 0) {
          log("'recv' success. connection closing");
        } else {
          outputBuffer = std::vector<char>(inputBuffer.begin(), inputBuffer.begin() + recvResult);
          log("'recv' success. bytes: '" + std::to_string(recvResult) + "'");
        }
      } while(recvResult > 0);

      log("'recv' success.");

      closesocket(connectSocket);
      WSACleanup();

      return outputBuffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketClient::log(const std::string& message) {
    if (logHandler) {
      logHandler(message);
    }
  }

}
