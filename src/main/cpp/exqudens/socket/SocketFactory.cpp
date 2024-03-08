#include <filesystem>
#include <stdexcept>

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#endif

#include "exqudens/socket/SocketFactory.hpp"
#include "exqudens/socket/SocketServer.hpp"
#include "exqudens/socket/SocketClient.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define LOG_TRACE 5

namespace exqudens {

  void SocketFactory::setLogFunction(
      const std::function<void(
          const std::string&,
          const unsigned short&,
          const std::string&,
          const std::string&,
          const size_t&
      )>& value
  ) {
    logFunction = value;
  }

  bool SocketFactory::isInitialized() {
    return initialized;
  }

  void SocketFactory::init() {
    try {

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

      WSADATA wsaData;

      int wsaStartupResult = WSAStartup(MAKEWORD(2,2), &wsaData);

      if (wsaStartupResult != 0) {
        std::string errorMessage = "'WSAStartup' failed with result: '";
        errorMessage += std::to_string(wsaStartupResult);
        errorMessage += "'";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }

#endif

      initialized = true;

      log("'init' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::shared_ptr<ISocket> SocketFactory::createSharedServer() {
    try {
      if (!initialized) {
        std::string errorMessage = "Not initialized. Call init(...) before usage";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }
      std::shared_ptr<ISocket> result = nullptr;
      result = std::make_shared<SocketServer>();
      if (logFunction) {
        result->setLogFunction(logFunction);
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::unique_ptr<ISocket> SocketFactory::createUniqueServer() {
    try {
      if (!initialized) {
        std::string errorMessage = "Not initialized. Call init(...) before usage";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }
      std::unique_ptr<ISocket> result = nullptr;
      result = std::make_unique<SocketServer>();
      if (logFunction) {
        result->setLogFunction(logFunction);
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::shared_ptr<ISocket> SocketFactory::createSharedClient() {
    try {
      if (!initialized) {
        std::string errorMessage = "Not initialized. Call init(...) before usage";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }
      std::shared_ptr<ISocket> result = nullptr;
      result = std::make_shared<SocketClient>();
      if (logFunction) {
        result->setLogFunction(logFunction);
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  std::unique_ptr<ISocket> SocketFactory::createUniqueClient() {
    try {
      if (!initialized) {
        std::string errorMessage = "Not initialized. Call init(...) before usage";
        throw std::runtime_error(CALL_INFO + ": " + errorMessage);
      }
      std::unique_ptr<ISocket> result = nullptr;
      result = std::make_unique<SocketClient>();
      if (logFunction) {
        result->setLogFunction(logFunction);
      }
      return result;
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketFactory::destroy() {
    try {
      if (initialized) {

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

        int wsaCleanupResult = WSACleanup();

        if (wsaCleanupResult != 0) {
          std::string errorMessage = "'WSACleanup' failed with result: '";
          errorMessage += std::to_string(wsaCleanupResult);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
        }

#endif

        initialized = false;

        log("'destroy' success.", LOG_INFO, __FUNCTION__, __FILE__, __LINE__);
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketFactory::log(
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
