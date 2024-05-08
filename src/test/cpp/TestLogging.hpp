#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <sstream>

#ifndef TEST_LOGGING_FATAL
#define TEST_LOGGING_FATAL(id) TestLogging::fatal(id, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef TEST_LOGGING_ERROR
#define TEST_LOGGING_ERROR(id) TestLogging::error(id, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef TEST_LOGGING_WARN
#define TEST_LOGGING_WARN(id) TestLogging::warn(id, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef TEST_LOGGING_INFO
#define TEST_LOGGING_INFO(id) TestLogging::info(id, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef TEST_LOGGING_DEBUG
#define TEST_LOGGING_DEBUG(id) TestLogging::debug(id, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef TEST_LOGGING_VERB
#define TEST_LOGGING_VERB(id) TestLogging::verb(id, __FILE__, __LINE__, __FUNCTION__)
#endif

class TestLogging {

  public:

    class Writer {

      private:

        std::string id;
        size_t level = 0;
        std::string file;
        size_t line = 0;
        std::string function;
        std::ostringstream stream;

      public:

        Writer(
          std::string id,
          size_t level,
          std::string file,
          size_t line,
          std::string function
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

    static Writer none(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

    static Writer fatal(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

    static Writer error(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

    static Writer warn(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

    static Writer info(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

    static Writer debug(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

    static Writer verb(const std::string& id, const std::string& file, const size_t& line, const std::string& function);

  private:

    static void log(const std::string& id, const size_t& level, const std::string& file, const size_t& line, const std::string& function, const std::string& message);

};
