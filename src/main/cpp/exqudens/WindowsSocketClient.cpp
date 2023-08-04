#include "exqudens/SocketClient.hpp"

#include <cstddef>
#include <string>
#include <stdexcept>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>

namespace exqudens {

  void SocketClient::setPort(const unsigned short& value) {
    port = value;
  }

  void SocketClient::setLogHandler(const std::function<void(const std::string&)>& value) {
    logHandler = value;
  }

  std::vector<char> SocketClient::exchange(const std::vector<char>& value) {
    try {
      WSADATA wsaData;
      int wsaDataResult = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (wsaDataResult != 0) {
        std::string errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(wsaDataResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'WSAStartup' success.");

      int wsaLastError = 0;
      size_t connectSocket = socket(AF_INET6, SOCK_STREAM, 0);

      if (connectSocket == INVALID_SOCKET) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'connect' failed with result: '";
        errorMessage += std::to_string(connectSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'socket' success.");

      unsigned long mode = 1L;
      int setSockOptResult = setsockopt(connectSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&mode), sizeof(mode));

      if (setSockOptResult != NO_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'setsockopt' failed with result: '";
        errorMessage += std::to_string(setSockOptResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'setsockopt' success.");

      int ioctlSocketResult = ioctlsocket(connectSocket, FIONBIO, &mode);

      if (ioctlSocketResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'ioctlsocket' failed with result: '";
        errorMessage += std::to_string(ioctlSocketResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'ioctlsocket' success.");

      SOCKADDR_STORAGE socketAddressStorage = {0};
      socketAddressStorage.ss_family = AF_INET6;
      INETADDR_SETLOOPBACK((SOCKADDR*) &socketAddressStorage);
      SS_PORT((SOCKADDR*) &socketAddressStorage) = htons(port);

      log("'htons' success.");

      int connectResult = connect(connectSocket, (SOCKADDR*) &socketAddressStorage, sizeof(socketAddressStorage));

      if (connectResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        if (wsaLastError != WSAEWOULDBLOCK) {
          closesocket(connectSocket);
          WSACleanup();
          std::string errorMessage = "'connect' failed with result: '";
          errorMessage += std::to_string(ioctlSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }
      }

      log("'connect' success.");

      WSAPOLLFD fdArray = {0};
      fdArray.fd = connectSocket;
      fdArray.events = POLLWRNORM;
      int timeout = (3 * 60 * 1000);

      int wsaPollOutResult = WSAPoll(&fdArray, 1, timeout);

      if (wsaPollOutResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (Out)' failed with result: '";
        errorMessage += std::to_string(wsaPollOutResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'WSAPoll (Out)' success.");

      if(!(fdArray.revents & POLLWRNORM)) {
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (Out)' read events '";
        errorMessage += std::to_string((fdArray.revents & POLLWRNORM));
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'WSAPoll (Out) read events' success.");

      int sendResult = send(connectSocket, value.data(), (int) value.size(), 0);

      if (sendResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'send' success.");

      fdArray.events = POLLRDNORM;
      int wsaPollInResult = WSAPoll(&fdArray, 1, timeout);

      if (wsaPollInResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (In)' failed with result: '";
        errorMessage += std::to_string(wsaPollInResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'WSAPoll (In)' success.");

      if(!(fdArray.revents & POLLRDNORM)) {
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (In)' read events '";
        errorMessage += std::to_string((fdArray.revents & POLLRDNORM));
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'WSAPoll (In) read events' success.");

      std::vector<char> inputBuffer(1024);
      int recvResult = recv(connectSocket, inputBuffer.data(), (int) inputBuffer.size(), 0);
      std::vector<char> outputBuffer;

      if (recvResult < 0 || recvResult > (int) inputBuffer.size()) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      } else if (recvResult > 0) {
        for (int i = 0; i < recvResult; i++) {
          outputBuffer.emplace_back(inputBuffer.at(i));
        }
      }

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
