#include "exqudens/SocketClient.hpp"

#include <cstddef>
#include <string>
#include <stdexcept>

#include <winsock2.h>
#include <ws2tcpip.h>

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

  void SocketClient::setSendBufferSize(const int& value) {
    sendBufferSize = value > 0 ? value : 1024;
  }

  void SocketClient::setReceiveBufferSize(const int& value) {
    receiveBufferSize = value > 0 ? value : 1024;
  }

  void SocketClient::connection() {
    try {
      std::string errorMessage;
      int wsaLastError;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      //size_t connectSocket;
      int connectResult;

      getAddrInfoResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addressInfo);

      if (getAddrInfoResult != 0) {
        errorMessage = "'getaddrinfo' failed with result: '";
        errorMessage += std::to_string(getAddrInfoResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      for(addrinfo* i = addressInfo; i != nullptr; i = i->ai_next) {
        connectSocket = socket(i->ai_family, i->ai_socktype, i->ai_protocol);

        if (connectSocket == INVALID_SOCKET) {
          wsaLastError = WSAGetLastError();
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
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(connectSocket);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'socket' success.");

      if (connectResult == SOCKET_ERROR) {
        errorMessage = "'connect' failed with result: '";
        errorMessage += std::to_string(connectResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'connect' success.");
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  int SocketClient::sendData(const std::vector<char>& value) {
    try {
      int wsaLastError;
      std::string errorMessage;

      if (value.size() > (size_t) sendBufferSize) {
        errorMessage = "'value.size' greater than sendBufferSize: '";
        errorMessage += std::to_string(sendBufferSize);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      if (value.empty()) {
        int shutdownResult = shutdown(connectSocket, SD_SEND);

        if (shutdownResult == SOCKET_ERROR) {
          wsaLastError = WSAGetLastError();
          closesocket(connectSocket);
          errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'shutdown' success.");

        return 0;
      } else {
        int sendResult = send(connectSocket, value.data(), (int) value.size(), 0);

        if (sendResult == SOCKET_ERROR) {
          wsaLastError = WSAGetLastError();
          closesocket(connectSocket);
          errorMessage = "'send' failed with result: '";
          errorMessage += std::to_string(sendResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(wsaLastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'send' success. bytes: '" + std::to_string(sendResult) + "'");

        return sendResult;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  std::vector<char> SocketClient::receiveData() {
    try {
      int wsaLastError;
      std::string errorMessage;
      std::vector<char> inputBuffer;
      int recvResult;
      std::vector<char> outputBuffer;

      inputBuffer = std::vector<char>(receiveBufferSize);
      recvResult = recv(connectSocket, inputBuffer.data(), (int) inputBuffer.size(), 0);

      if (recvResult < 0) {
        wsaLastError = WSAGetLastError();
        closesocket(connectSocket);
        errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(wsaLastError);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      } else if (recvResult == 0) {
        log("'recv' success. connection closing");
        outputBuffer = std::vector<char>();
      } else {
        outputBuffer = std::vector<char>(inputBuffer.begin(), inputBuffer.begin() + recvResult);
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'");
      }

      return outputBuffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketClient::disconnection() {
    try {
      closesocket(connectSocket);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  std::vector<char> SocketClient::exchange(const std::vector<char>& value) {
    try {
      std::vector<char> inputBuffer;
      std::vector<char> outputBuffer;

      if (value.empty()) {
        return outputBuffer;
      }

      connection();

      if (value.size() > (size_t) sendBufferSize) {
        for (size_t i = 0; i < value.size(); i += sendBufferSize) {
          outputBuffer = {};
          for (size_t j = 0; j < sendBufferSize && i + j < value.size(); j++) {
            outputBuffer.emplace_back(value.at(i + j));
          }
          sendData(outputBuffer);
          log("sendData size: '" + std::to_string(outputBuffer.size()) + "'");
        }
        sendData({});
        log("sendData size: '0'");
      } else {
        sendData(value);
        log("sendData size: '" + std::to_string(value.size()) + "'");
        sendData({});
        log("sendData size: '0'");
      }

      outputBuffer = {};
      size_t received;

      do {
        inputBuffer = receiveData();
        log("receiveData size: '" + std::to_string(inputBuffer.size()) + "'");

        if (!inputBuffer.empty()) {
          for (char c : inputBuffer) {
            outputBuffer.emplace_back(c);
          }
        }

        received = inputBuffer.size();
      } while (received > 0);

      disconnection();

      return outputBuffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void SocketClient::log(const std::string& message) {
    try {
      if (logHandler) {
        logHandler(message);
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

}
