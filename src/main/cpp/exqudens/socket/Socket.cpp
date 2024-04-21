#include <filesystem>
#include <stdexcept>

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#else
#include <netdb.h>
#endif

#include "exqudens/socket/Socket.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define LOG_TRACE 5

namespace exqudens {

  void Socket::setPort(const unsigned short& value) {
    port = value;
  }

  void Socket::setHost(const std::string& value) {
    throw std::runtime_error(CALL_INFO + ": Not applicable!!!");
  }

  void Socket::setLogFunction(const std::function<void( const std::string&, const unsigned short&, const std::string&, const std::string&, const size_t&)>& value) {
    logFunction = value;
  }

  void Socket::init() {
    throw std::runtime_error(CALL_INFO + ": Not applicable!!!");
  }

  size_t Socket::sendData(const std::vector<char>& buffer) {
    try {
      if (buffer.size() > INT_MAX) {
        std::string errorMessage = "'buffer' size: '";
        errorMessage += std::to_string(buffer.size());
        errorMessage += "' is greater than max buffer size: '";
        errorMessage += std::to_string(INT_MAX);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      size_t result = 0;

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      int sendResult = send(transferSocket.load(), buffer.data(), (int) buffer.size(), 0);

      if (sendResult < 0) {
        int lastError = WSAGetLastError();
        destroy();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      result = sendResult;

      log("'send' success. bytes: '" + std::to_string(result) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

#else

      ssize_t sendResult = send((int) transferSocket.load(), buffer.data(), (int) buffer.size(), 0);

      if (sendResult < 0) {
        int lastError = errno;
        destroy();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      result = sendResult;

      log("'send' success. bytes: '" + std::to_string(result) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);

#endif

      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::vector<char> Socket::receiveData(const size_t& bufferSize) {
    try {
      std::vector<char> buffer = std::vector<char>(bufferSize > 0 ? bufferSize : 1024);

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      int recvResult = recv(transferSocket.load(), buffer.data(), (int) buffer.size(), 0);

      if (recvResult < 0) {
        int lastError = WSAGetLastError();
        destroy();
        std::string errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }  else if (recvResult == 0) {
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      } else {
        buffer.resize(recvResult);
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }

#else

      ssize_t recvResult = recv((int) transferSocket.load(), buffer.data(), (int) buffer.size(), 0);

      if (recvResult < 0) {
        int lastError = errno;
        destroy();
        std::string errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }  else if (recvResult == 0) {
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      } else {
        buffer.resize(recvResult);
        log("'recv' success. bytes: '" + std::to_string(recvResult) + "'", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }

#endif

      return buffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::vector<char> Socket::receiveData() {
    return receiveData(1024);
  }

  void Socket::destroy() {
    throw std::runtime_error(CALL_INFO + ": Not applicable!!!");
  }

  void Socket::log(
      const std::string& message,
      const unsigned short& level,
      const std::string& function,
      const std::string& file,
      const size_t& line
  ) {
    try {
      if (logFunction) {
        logFunction(message, level, function, std::filesystem::path(file).filename().string(), line);
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
#undef LOG_FATAL
#undef LOG_ERROR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_TRACE
