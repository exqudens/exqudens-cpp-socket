#include <filesystem>
#include <stdexcept>

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)
#include <winsock2.h>
#endif

#include "exqudens/socket/SocketFactory.hpp"
#include "exqudens/socket/SocketServer.hpp"
#include "exqudens/socket/SocketClient.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOG_FATAL 1
#define LOG_ERROR 2
#define LOG_WARNING 3
#define LOG_INFO 4
#define LOG_DEBUG 5
#define LOG_VERBOSE 6

namespace exqudens {

  void SocketFactory::setLogFunction(
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

  bool SocketFactory::isInitialized() {
    return initialized;
  }

  void SocketFactory::init() {
    try {
      if (!initialized) {

#if defined(_WIN64) || defined(_WIN32) || defined(_WINDOWS)

        WSADATA wsaData;

        int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

        if (wsaStartupResult != 0) {
          std::string errorMessage = "'WSAStartup' failed with result: '";
          errorMessage += std::to_string(wsaStartupResult);
          errorMessage += "'";
          throw std::runtime_error(CALL_INFO + ": " + errorMessage);
        }

#endif

        log(__FILE__, __LINE__, __FUNCTION__, __FILE__, LOG_INFO, "'init' success.");

        initialized = true;
      }
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
      std::shared_ptr<ISocket> result(new SocketServer());
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
      std::unique_ptr<ISocket> result(new SocketServer());
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
      std::shared_ptr<ISocket> result(new SocketClient());
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
      std::unique_ptr<ISocket> result(new SocketClient("exqudens.SocketClient"));
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

        log(__FILE__, __LINE__, __FUNCTION__, __FILE__, LOG_INFO, "'destroy' success.");

        initialized = false;
      }
    } catch (...) {
      std::throw_with_nested(std::runtime_error(CALL_INFO));
    }
  }

  void SocketFactory::log(
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
            std::filesystem::path(id).filename().replace_extension().string(),
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
