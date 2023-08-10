#include "exqudens/SocketClient.hpp"

#include <cstddef>
#include <cstring>
#include <string>
#include <stdexcept>

#include <netdb.h>
#include <unistd.h>

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

  void SocketClient::init() {
    try {
      std::string errorMessage;
      int lastError;
      addrinfo hints = {};
      addrinfo* addressInfo = nullptr;
      int getAddrInfoResult;
      int socketResult;
      //size_t connectSocket;
      int connectResult;

      std::memset(&hints, 0, sizeof(hints));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = IPPROTO_TCP;

      getAddrInfoResult = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addressInfo);

      if (getAddrInfoResult != 0) {
        errorMessage = "'getaddrinfo' failed with result: '";
        errorMessage += std::to_string(getAddrInfoResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'getaddrinfo' success.");

      for(addrinfo* i = addressInfo; i != nullptr; i = i->ai_next) {
        socketResult = socket(i->ai_family, i->ai_socktype, i->ai_protocol);

        if (socketResult < 0) {
          lastError = errno;
          errorMessage = "'socket' failed with result: '";
          errorMessage += std::to_string(socketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        connectSocket = socketResult;

        log("'socket' success. connectSocket: '" + std::to_string(connectSocket) + "'");

        connectResult = connect((int) connectSocket, i->ai_addr, (int) i->ai_addrlen);

        if (connectResult < 0) {
          close((int) connectSocket);
          connectSocket = ~0;
          continue;
        }

        break;
      }

      freeaddrinfo(addressInfo);

      if (connectSocket == ((size_t) ~0)) {
        errorMessage = "'socket' failed with result: '";
        errorMessage += std::to_string(connectSocket);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      log("'socket' success.");

      if (connectResult < 0) {
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

  int SocketClient::sendData(const std::vector<char>& value, const int& bufferSize) {
    try {
      size_t internalBufferSize = bufferSize > 0 ? bufferSize : 1024;
      for (size_t i = 0; i < value.size(); i += internalBufferSize) {
        std::vector<char> outputBuffer = {};
        for (size_t j = 0; j < internalBufferSize && i + j < value.size(); j++) {
          outputBuffer.emplace_back(value.at(i + j));
        }

        ssize_t sendResult = send((int) connectSocket, outputBuffer.data(), (int) outputBuffer.size(), 0);

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
      }
      return 0;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  std::vector<char> SocketClient::receiveData(const int& bufferSize) {
    try {
      std::vector<char> buffer = std::vector<char>(bufferSize > 0 ? bufferSize : 1024);
      ssize_t recvResult = recv((int) connectSocket, buffer.data(), (int) buffer.size(), 0);

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

  void SocketClient::destroy() {
    try {
      if (connectSocket != ((size_t) ~0)) {
        size_t tmpConnectSocket = connectSocket;
        connectSocket = ~0;

        int shutdownResult = shutdown((int) tmpConnectSocket, SHUT_WR);

        if (shutdownResult < 0) {
          int lastError = errno;
          close((int) tmpConnectSocket);
          std::string errorMessage = "'shutdown' failed with result: '";
          errorMessage += std::to_string(shutdownResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'shutdown' success. connectSocket: '" + std::to_string(tmpConnectSocket) + "'");

        int closeSocketResult = close((int) tmpConnectSocket);

        if (closeSocketResult < 0) {
          int lastError = errno;
          std::string errorMessage = "'closesocket' failed with result: '";
          errorMessage += std::to_string(closeSocketResult);
          errorMessage += "' error: '";
          errorMessage += std::to_string(lastError);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        log("'closesocket' success. connectSocket: '" + std::to_string(tmpConnectSocket) + "'");
      }
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
