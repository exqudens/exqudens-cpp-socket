#include <filesystem>
#include <stdexcept>

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>

typedef SSIZE_T ssize_t;
#else
#include <netdb.h>

typedef int SOCKET;
#endif

#include "exqudens/socket/Socket.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOG_FATAL 1
#define LOG_ERROR 2
#define LOG_WARNING 3
#define LOG_INFO 4
#define LOG_DEBUG 5
#define LOG_VERBOSE 6

namespace exqudens {

  Socket::Socket(
      const std::string& loggerId
  ):
      loggerId(loggerId)
  {}

  void Socket::setPort(const unsigned short& value) {
    port = value;
  }

  void Socket::setLogFunction(
      const std::function<void(
          const std::string&,
          const size_t&,
          const std::string&,
          const std::string&,
          const unsigned short&,
          const std::string&
      )>& value
  ) {
    logFunction = value;
  }

  size_t Socket::sendData(const std::vector<char>& buffer) {
    try {
      size_t result = 0;

      if (buffer.empty()) {
        return result;
      } else if (buffer.size() > INT_MAX) {
        std::string errorMessage = "'buffer' size: '";
        errorMessage += std::to_string(buffer.size());
        errorMessage += "' is greater than max buffer size: '";
        errorMessage += std::to_string(INT_MAX);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      ssize_t sendResult = send((SOCKET) transferSocket.load(), buffer.data(), (int) buffer.size(), 0);

      if (sendResult < 0) {
        int lastError = getLastError();
        destroy();
        std::string errorMessage = "'send' failed with result: '";
        errorMessage += std::to_string(sendResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

      result = sendResult;

      log(__FILE__, __LINE__, __FUNCTION__, loggerId, LOG_INFO, "'send' success. bytes: '" + std::to_string(result) + "'");

      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::vector<char> Socket::receiveData(const size_t& bufferSize) {
    try {
      std::vector<char> buffer = std::vector<char>(bufferSize > 0 ? bufferSize : 1024);

      ssize_t recvResult = recv((SOCKET) transferSocket.load(), buffer.data(), (int) buffer.size(), 0);

      if (recvResult < 0) {
        int lastError = getLastError();
        destroy();
        std::string errorMessage = "'recv' failed with result: '";
        errorMessage += std::to_string(recvResult);
        errorMessage += "' error: '";
        errorMessage += std::to_string(lastError);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }  else if (recvResult == 0) {
        log(__FILE__, __LINE__, __FUNCTION__, loggerId, LOG_INFO, "'recv' success. bytes: '" + std::to_string(recvResult) + "'");
      } else {
        buffer.resize(recvResult);
        log(__FILE__, __LINE__, __FUNCTION__, loggerId, LOG_INFO, "'recv' success. bytes: '" + std::to_string(recvResult) + "'");
      }

      return buffer;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::vector<char> Socket::receiveData() {
    return receiveData(1024);
  }

  int Socket::getLastError() {
    try {
      int result = 0;

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      result = WSAGetLastError();

#else

      result = errno;

#endif

      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::pair<size_t, std::string> Socket::openSocket(const int& family, const int& type, const int& protocol) {
    try {
      size_t first = SIZE_MAX;
      std::string second = "-1";

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      size_t socketResult = socket(family, type, protocol);
      first = socketResult;
      second = std::to_string(socketResult);

#else

      int socketResult = socket(family, type, protocol);
      first = socketResult < 0 ? SIZE_MAX : socketResult;
      second = std::to_string(socketResult);

#endif

      return {first, second};
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::pair<size_t, std::string> Socket::acceptSocket(const size_t& value) {
    try {
      size_t first = SIZE_MAX;
      std::string second = "-1";

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      size_t acceptResult = accept((SOCKET) listenSocket.load(), nullptr, nullptr);
      first = acceptResult;
      second = std::to_string(acceptResult);

#else

      int acceptResult = accept((SOCKET) listenSocket.load(), nullptr, nullptr);
      first = acceptResult < 0 ? SIZE_MAX : acceptResult;
      second = std::to_string(acceptResult);

#endif

      return {first, second};
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  int Socket::closeSocket(const size_t& value) {
    try {
      int result = 0;

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      result = closesocket((SOCKET) value);

#else

      result = close((SOCKET) value);

#endif

      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  int Socket::shutdownSocket(const size_t& value) {
    try {
      int how = 2; // (winsock2.h SD_BOTH) == (netdb.h SHUT_RDWR);
      int result = shutdown((SOCKET) value, how);
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void Socket::log(
      const std::string& file,
      const size_t& line,
      const std::string& function,
      const std::string& id,
      const unsigned short& level,
      const std::string& message
  ) {
    try {
      if (logFunction) {
        logFunction(
            std::filesystem::path(file).filename().string(),
            line,
            function,
            id,
            level,
            message
        );
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

}

#undef CALL_INFO
#undef LOG_FATAL
#undef LOG_ERROR
#undef LOG_WARNING
#undef LOG_INFO
#undef LOG_DEBUG
#undef LOG_VERBOSE
