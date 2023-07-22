#include "exqudens/SocketClient.hpp"

#include <string>
#include <stdexcept>

#include <winsock2.h>
#include <ws2tcpip.h>

namespace exqudens {

  SocketClient& SocketClient::setPort(const unsigned int& value) {
    port = value;
    return *this;
  }

  int SocketClient::sendData(const std::vector<char>& value) {
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
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;
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

      addrinfo* addressInfoTmp = nullptr;
      size_t connectSocket = INVALID_SOCKET;

      for(addressInfoTmp = addressInfo; addressInfoTmp != nullptr; addressInfoTmp = addressInfoTmp->ai_next) {
        connectSocket = socket(addressInfoTmp->ai_family, addressInfoTmp->ai_socktype, addressInfoTmp->ai_protocol);

        if (connectSocket == INVALID_SOCKET) {
          wsaLastError = WSAGetLastError();
          WSACleanup();
          std::string errorMessage = "'socket' failed with result: '";
          errorMessage += std::to_string(connectSocket);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        result = connect(connectSocket, addressInfoTmp->ai_addr, (int) addressInfoTmp->ai_addrlen);

        if (result == SOCKET_ERROR) {
          closesocket(connectSocket);
          connectSocket = INVALID_SOCKET;
          continue;
        }
        break;
      }

      freeaddrinfo(addressInfo);

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

      result = send(connectSocket, value.data(), (int) value.size(), 0);

      if (result == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      result = shutdown(connectSocket, SD_SEND);

      if (result == SOCKET_ERROR) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        WSACleanup();
        std::string errorMessage = "'shutdown' failed with result: '";
        errorMessage += std::to_string(result);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      int bufferSize = 1024;
      std::vector<char> outputBuffer;

      do {
        std::vector<char> buffer(bufferSize);
        result = recv(connectSocket, buffer.data(), bufferSize, 0);

        if (result < 0 || result > bufferSize) {
          wsaLastError = WSAGetLastError();
          closesocket(connectSocket);
          WSACleanup();
          std::string errorMessage = "'recv' failed with result: '";
          errorMessage += std::to_string(result);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        } else if (result > 0) {
          for (int i = 0; i < result; i++) {
            outputBuffer.emplace_back(buffer.at(i));
          }
        }
      } while (result > 0);

      closesocket(connectSocket);
      WSACleanup();

      int output = 0;
      std::memcpy(&output, outputBuffer.data(), sizeof(int));
      return output;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

}
