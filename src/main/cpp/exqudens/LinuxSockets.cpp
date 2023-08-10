#include "exqudens/Sockets.hpp"

#include <stdexcept>

namespace exqudens {

    void Sockets::setLogHandler(const std::function<void(const std::string&)>& value) {
        logHandler = value;
    }

    void Sockets::init() {
        try {
            initialized = true;

            log("'init' success.");
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
                initialized = false;

                log("'destroy' success.");
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
