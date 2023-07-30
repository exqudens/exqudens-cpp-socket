#include "exqudens/SocketServer.hpp"

#include <cstddef>
#include <string>
#include <stdexcept>
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>

namespace exqudens {

  void SocketServer::setPort(const unsigned short& value) {
    port = value;
  }

  void SocketServer::setReceiveHandler(const std::function<std::vector<char>(const std::vector<char>&)>& function) {
    receiveHandler = function;
  }

  void SocketServer::start() {
    try {
      WSADATA wsaData;
      int wsaStartupResult = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (wsaStartupResult != 0) {
        std::string errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(wsaStartupResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'WSAStartup' success." << std::endl;

      SOCKADDR_STORAGE socketAddressStorage = {0};
      socketAddressStorage.ss_family = AF_INET6;
      INETADDR_SETANY((SOCKADDR*) &socketAddressStorage);
      SS_PORT((SOCKADDR*) &socketAddressStorage) = htons(port);

      std::cout << "[server] 'htons' success." << std::endl;

      int wsaLastError = 0;
      size_t listenSocket = socket(AF_INET6, SOCK_STREAM, 0);

      if (listenSocket == INVALID_SOCKET) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(listenSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'socket' success." << std::endl;

      unsigned long mode = 1L;
      int setSockOptResult = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&mode), sizeof(mode));

      if (setSockOptResult != NO_ERROR) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'setsockopt' failed with result: '";
        errorMessage += std::to_string(setSockOptResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'setsockopt' success." << std::endl;

      int ioctlSocketResult = ioctlsocket(listenSocket, FIONBIO, &mode);

      if (ioctlSocketResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'ioctlsocket' failed with result: '";
        errorMessage += std::to_string(ioctlSocketResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'ioctlsocket' success." << std::endl;

      int bindResult = bind(listenSocket, (SOCKADDR*) &socketAddressStorage, sizeof(socketAddressStorage));

      if (bindResult != NO_ERROR) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'bind' failed with result: '";
        errorMessage += std::to_string(bindResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'bind' success." << std::endl;

      int listenResult = listen(listenSocket, 1);

      if (listenResult != NO_ERROR) {
        wsaLastError = WSAGetLastError();
        WSACleanup();
        std::string errorMessage = "'listen' failed with result: '";
        errorMessage += std::to_string(listenResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'listen' success." << std::endl;

      WSAPOLLFD fdArray = {0};
      fdArray.fd = listenSocket;
      fdArray.events = POLLRDNORM;
      int timeout = (3 * 60 * 1000);
      unsigned long fds = 1L;

      int wsaPollInResult = WSAPoll(&fdArray, fds, timeout);

      if (wsaPollInResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (In)' failed with result: '";
        errorMessage += std::to_string(wsaPollInResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'WSAPoll (In)' success." << std::endl;

      if(!(fdArray.revents & POLLRDNORM)) {
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (In)' read events '";
        errorMessage += std::to_string(fdArray.revents);
        errorMessage += "' not equal to '";
        errorMessage += std::to_string(POLLRDNORM);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'WSAPoll (In) read events' success." << std::endl;

      size_t acceptedSocket = accept(listenSocket, nullptr, nullptr);

      if (acceptedSocket == INVALID_SOCKET) {
        wsaLastError = WSAGetLastError();
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'accept' failed with result: '";
        errorMessage += std::to_string(acceptedSocket);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'accept' success." << std::endl;

      std::vector<char> inputBuffer(1024);
      int recvResult = recv(acceptedSocket, inputBuffer.data(), (int) inputBuffer.size(), 0);
      std::vector<char> outputBuffer;

      if (recvResult < 0 || recvResult > (int) inputBuffer.size()) {
        wsaLastError = WSAGetLastError();
        closesocket(acceptedSocket);
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      } else if (recvResult > 0) {
        std::vector<char> tmpBuffer = std::vector<char>(inputBuffer.begin(), inputBuffer.begin() + recvResult);
        outputBuffer = receiveHandler(tmpBuffer);
      }

      std::cout << "[server] 'recv' success." << std::endl;

      fdArray.fd = acceptedSocket;
      fdArray.events = POLLWRNORM;
      int wsaPollOutResult = WSAPoll(&fdArray, 1, timeout);

      if (wsaPollOutResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(acceptedSocket);
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (Out)' failed with result: '";
        errorMessage += std::to_string(wsaPollOutResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'WSAPoll (Out)' success." << std::endl;

      if(!(fdArray.revents & POLLWRNORM)) {
        closesocket(acceptedSocket);
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'WSAPoll (Out)' read events '";
        errorMessage += std::to_string(fdArray.revents);
        errorMessage += "' not equal to '";
        errorMessage += std::to_string(POLLWRNORM);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'WSAPoll (Out) read events' success." << std::endl;

      int sendResult = send(acceptedSocket, outputBuffer.data(), (int) outputBuffer.size(), 0);

      if (sendResult == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(acceptedSocket);
        closesocket(listenSocket);
        WSACleanup();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      std::cout << "[server] 'send' success." << std::endl;

      closesocket(acceptedSocket);
      closesocket(listenSocket);
      WSACleanup();
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketServer::stop() {
    stoped = true;
  }

}
