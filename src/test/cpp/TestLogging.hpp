#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <sstream>

#ifndef TEST_LOG_F
#define TEST_LOG_F(id) TestLogging::Writer(__FILE__, __LINE__, __FUNCTION__, id, TestLogging::Level::FATAL)
#endif

#ifndef TEST_LOG_E
#define TEST_LOG_E(id) TestLogging::Writer(__FILE__, __LINE__, __FUNCTION__, id, TestLogging::Level::ERROR)
#endif

#ifndef TEST_LOG_W
#define TEST_LOG_W(id) TestLogging::Writer(__FILE__, __LINE__, __FUNCTION__, id, TestLogging::Level::WARNING)
#endif

#ifndef TEST_LOG_I
#define TEST_LOG_I(id) TestLogging::Writer(__FILE__, __LINE__, __FUNCTION__, id, TestLogging::Level::INFO)
#endif

#ifndef TEST_LOG_D
#define TEST_LOG_D(id) TestLogging::Writer(__FILE__, __LINE__, __FUNCTION__, id, TestLogging::Level::DEBUG)
#endif

#ifndef TEST_LOG_V
#define TEST_LOG_V(id) TestLogging::Writer(__FILE__, __LINE__, __FUNCTION__, id, TestLogging::Level::VERBOSE)
#endif

class TestLogging {

  public:

    struct Level {
      inline static const unsigned short FATAL = 1;
      inline static const unsigned short ERROR = 2;
      inline static const unsigned short WARNING = 3;
      inline static const unsigned short INFO = 4;
      inline static const unsigned short DEBUG = 5;
      inline static const unsigned short VERBOSE = 6;
    };

    class Writer {

      private:

        std::string file;
        size_t line = 0;
        std::string function;
        std::string id;
        unsigned short level = 0;
        std::ostringstream stream;

      public:

        Writer(
          std::string file,
          size_t line,
          std::string function,
          std::string id,
          unsigned short level
        );

        Writer() = delete;
        Writer(const Writer&) = delete;
        Writer& operator=(const Writer&) = delete;

        template <typename T>
        inline Writer& operator<<(const T& value) {
          stream << value;
          return *this;
        }

        ~Writer();

    };

  private:

    inline static bool configured = false;

  public:

    static std::string defaultConfig();

    static std::string defaultGlobalConfig();

    static void config(const std::string& file, const std::string& workingDir = "");

    static std::string config(const std::vector<std::string>& commandLineArgs);

  private:

    static void log(
        const std::string& file,
        const size_t& line,
        const std::string& function,
        const std::string& id,
        const unsigned short& level,
        const std::string& message
    );

};
