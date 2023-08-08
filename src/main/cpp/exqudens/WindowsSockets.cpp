#include "exqudens/Sockets.hpp"

#include <stdexcept>

#include <winsock2.h>

namespace exqudens {

  void Sockets::setLogHandler(const std::function<void(const std::string&)>& value) {
    logHandler = value;
  }

  void Sockets::init() {
    try {
      WSADATA wsaData;

      int wsaStartupResult = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (wsaStartupResult != 0) {
        std::string errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(wsaStartupResult);
        errorMessage += "'";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }

      initialized = true;

      log("'WSAStartup' success.");
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  SocketServer Sockets::createServer() {
    try {
      if (!initialized) {
        std::string errorMessage = "Not initialized. Call init(...) before usage";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }
      SocketServer result;
      if (logHandler) {
        result.setLogHandler([](const std::string& message) {
          logHandler("[SERVER] " + message);
        });
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  SocketClient Sockets::createClient() {
    try {
      if (!initialized) {
        std::string errorMessage = "Not initialized. Call init(...) before usage";
        throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
      }
      SocketClient result;
      if (logHandler) {
        result.setLogHandler([](const std::string& message) {
          logHandler("[CLIENT] " + message);
        });
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void Sockets::destroy() {
    try {
      if (initialized) {
        int wsaCleanupResult = WSACleanup();

        if (wsaCleanupResult != 0) {
          std::string errorMessage = "'WSACleanup' failed with result: '";
          errorMessage += std::to_string(wsaCleanupResult);
          errorMessage += "'";
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): " + errorMessage);
        }

        initialized = false;

        log("'WSACleanup' success.");
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

  void Sockets::log(const std::string& message) {
    try {
      if (logHandler) {
        logHandler(message);
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
    }
  }

}
