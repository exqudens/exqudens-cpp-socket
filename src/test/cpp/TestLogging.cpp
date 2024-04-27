#include <filesystem>
#include <stdexcept>

#include "TestLogging.hpp"
INITIALIZE_EASYLOGGINGPP

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

std::string TestLogging::defaultConfig() {
  try {
    std::string value;

    value += "* GLOBAL:\n";
    value += "  ENABLED              = true\n";
    value += "  TO_STANDARD_OUTPUT   = true\n";
    value += "  TO_FILE              = false\n";
    value += "  FILENAME             = \"log.txt\"\n";
    value += "  MAX_LOG_FILE_SIZE    = 2097152 ## 2MB\n";
    value += "  LOG_FLUSH_THRESHOLD  = 1 ## Flush after every 1 log\n";
    value += "  FORMAT               = \"%datetime %level [%logger] %msg\"\n";
    value += "  MILLISECONDS_WIDTH   = 3\n";
    value += "  PERFORMANCE_TRACKING = false\n";

    return value;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestLogging::defaultGlobalConfig() {
  try {
    std::string value;

    value += "## \"default\" logger configurations\n";
    value += "-- default\n";
    value += defaultConfig();

    return value;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestLogging::config(const std::string& filePath, const std::string& workingDir) {
  try {
    std::filesystem::path file;

    if (workingDir.empty()) {
      file = std::filesystem::path(filePath);
    } else {
      file = std::filesystem::path(std::filesystem::path(workingDir) / std::filesystem::path(filePath));
    }

    if (!std::filesystem::exists(file)) {
      throw std::runtime_error(CALL_INFO + ": Not exists: '" + file.generic_string() + "'");
    }

    el::Configurations configurations;

    configurations.setToDefault();

    configurations.setGlobally(el::ConfigurationType::Enabled, "true");
    configurations.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    configurations.setGlobally(el::ConfigurationType::ToFile, "false");
    configurations.setGlobally(el::ConfigurationType::Filename, "log.txt");
    configurations.setGlobally(el::ConfigurationType::MaxLogFileSize, "2097152");
    configurations.setGlobally(el::ConfigurationType::LogFlushThreshold, "1");
    configurations.setGlobally(el::ConfigurationType::Format, "%datetime %level [%logger] %msg");
    configurations.setGlobally(el::ConfigurationType::MillisecondsWidth, "3");
    configurations.setGlobally(el::ConfigurationType::PerformanceTracking, "false");

    el::Loggers::setDefaultConfigurations(configurations, true);

    el::Loggers::configureFromGlobal(file.generic_string().c_str());
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

#undef CALL_INFO
