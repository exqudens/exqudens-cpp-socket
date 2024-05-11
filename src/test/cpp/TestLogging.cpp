#include <optional>
#include <filesystem>
#include <stdexcept>
#include <utility>

#define ELPP_NO_DEFAULT_LOG_FILE
//#define ELPP_THREAD_SAFE
#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP

#include "TestLogging.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGING_CONFIG std::string("--logging-config")

TestLogging::Writer::Writer(
    std::string file,
    size_t line,
    std::string function,
    std::string id,
    unsigned short level
):
    file(std::move(file)),
    line(line),
    function(std::move(function)),
    id(std::move(id)),
    level(level)
{}

TestLogging::Writer::~Writer() {
  TestLogging::log(
      file,
      line,
      function,
      id,
      level,
      stream.str()
  );
}

std::string TestLogging::defaultConfig() {
  try {
    std::string value;

    value += "* GLOBAL:\n";
    value += "  ENABLED              = true\n";
    value += "  TO_STANDARD_OUTPUT   = true\n";
    value += "  TO_FILE              = false\n";
    value += "  FILENAME             = \"log-%datetime{%Y-%M-%d_%H-%m-%s}.txt\"\n";
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
    if (configured) {
      return;
    }

    std::filesystem::path file;

    if (workingDir.empty()) {
      file = std::filesystem::path(filePath);
    } else {
      file = std::filesystem::path(std::filesystem::path(workingDir) / std::filesystem::path(filePath));
    }

    if (!std::filesystem::exists(file)) {
      throw std::runtime_error(CALL_INFO + ": Not exists: '" + file.generic_string() + "'");
    }

    /*el::Configurations configurations;

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

    el::Loggers::configureFromGlobal(file.generic_string().c_str());*/

    el::Loggers::configureFromGlobal(file.generic_string().c_str());
    el::Configurations* configurations = el::Loggers::getLogger("default")->configurations();
    el::Loggers::setDefaultConfigurations(*configurations, true);
    el::Loggers::configureFromGlobal(file.generic_string().c_str());

    configured = true;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestLogging::config(const std::vector<std::string>& commandLineArgs) {
  try {
    std::string configType = "configured";

    if (configured) {
      return configType;
    }

    std::optional<std::string> loggingConfigFile = {};

    // try command line args
    for (size_t i = 0; i < commandLineArgs.size(); i++) {
      if (LOGGING_CONFIG == commandLineArgs.at(i) && (i + 1) <= (commandLineArgs.size() - 1)) {
        std::filesystem::path loggingFile(commandLineArgs.at(i + 1));
        if (std::filesystem::exists(loggingFile)) {
          loggingConfigFile = loggingFile.generic_string();
          configType = "command-line-arg (file: '" + loggingFile.generic_string() + "')";
        }
      }
    }
    // try executable dir
    if (!loggingConfigFile.has_value()) {
      std::filesystem::path executableDir = std::filesystem::path(commandLineArgs.front()).parent_path();
      std::filesystem::path loggingFile = executableDir / "logging-config.txt";
      if (std::filesystem::exists(loggingFile)) {
        loggingConfigFile = loggingFile.generic_string();
        configType = "executable-dir (file: '" + loggingFile.generic_string() + "')";
      }
    }
    // try src test resources
    if (!loggingConfigFile.has_value()) {
      std::filesystem::path currentFile = std::filesystem::path(__FILE__);
      if (
          !currentFile.empty()
          && std::filesystem::exists(currentFile)
          && currentFile.parent_path().filename().string() == "cpp"
          && currentFile.parent_path().parent_path().filename().string() == "test"
          && currentFile.parent_path().parent_path().parent_path().filename().string() == "src"
      ) {
        std::filesystem::path loggingFile = std::filesystem::path(currentFile.parent_path().parent_path()) / "resources" / "logging-config.txt";
        if (std::filesystem::exists(loggingFile)) {
          loggingConfigFile = loggingFile.generic_string();
          configType = "src-test-resources (file: '" + loggingFile.generic_string() + "')";
        }
      }
    }

    if (!loggingConfigFile.has_value()) {
      throw std::runtime_error(CALL_INFO + ": Can't find file: 'logging-config.txt'");
    }

    config(loggingConfigFile.value(), std::filesystem::path(commandLineArgs.front()).parent_path().generic_string());

    return configType;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestLogging::log(
    const std::string& file,
    const size_t& line,
    const std::string& function,
    const std::string& id,
    const unsigned short& level,
    const std::string& message
) {
  try {
    el::Level internalLevel = el::Level::Unknown;
    std::string internalFile = std::filesystem::path(file).filename().string();
    if (level == 1) {
      internalLevel = el::Level::Fatal;
    } else if (level == 2) {
      internalLevel = el::Level::Error;
    } else if (level == 3) {
      internalLevel = el::Level::Warning;
    } else if (level == 4) {
      internalLevel = el::Level::Info;
    } else if (level == 5) {
      internalLevel = el::Level::Debug;
    } else if (level == 6) {
      internalLevel = el::Level::Verbose;
    }
    el::base::Writer(internalLevel, internalFile.c_str(), line, function.c_str(), el::base::DispatchAction::NormalLog).construct(1, id.c_str()) << message;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

#undef CALL_INFO
#undef LOGGING_CONFIG
