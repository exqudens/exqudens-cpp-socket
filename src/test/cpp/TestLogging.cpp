#include <cstddef>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <nlohmann/json.hpp>

#include "TestLogging.hpp"
INITIALIZE_EASYLOGGINGPP

void TestLogging::config(const std::string& value) {
  try {
    el::Configurations loggingDefaultConfig;

    loggingDefaultConfig.setToDefault();

    loggingDefaultConfig.setGlobally(el::ConfigurationType::Enabled, "true");
    loggingDefaultConfig.setGlobally(el::ConfigurationType::ToFile, "true");
    loggingDefaultConfig.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    //loggingConfig.setGlobally(el::ConfigurationType::Format, "true");
    //loggingConfig.setGlobally(el::ConfigurationType::Filename, "true");
    //loggingConfig.setGlobally(el::ConfigurationType::MillisecondsWidth, "true");
    loggingDefaultConfig.setGlobally(el::ConfigurationType::PerformanceTracking, "false");
    //loggingConfig.setGlobally(el::ConfigurationType::MaxLogFileSize, "true");
    //loggingConfig.setGlobally(el::ConfigurationType::LogFlushThreshold, "true");

    el::Loggers::setDefaultConfigurations(loggingDefaultConfig, true);

    //el::Loggers::reconfigureLogger("default", loggingDefaultConfig);

    //el::Loggers::getLogger("exqudens.Tests");

    if (value.ends_with(".json")) {
      std::filesystem::path file(value);
      if (std::filesystem::exists(file)) {
        std::string content;
        std::ifstream inputStream(file.make_preferred().string(), std::ios::binary);
        if (inputStream.is_open()) {
          std::ostringstream buffer;
          buffer << inputStream.rdbuf();
          content = buffer.str();
          inputStream.close();
        } else {
          throw std::runtime_error(std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + "): Failed to open file: '" + file.make_preferred().string() + "'");
        }
        if (!content.empty()) {
          nlohmann::json json = nlohmann::json::parse(content);
          for (size_t i = 0; i < json.at("loggers").size(); i++) {
            nlohmann::json logger = json.at("loggers").at(i);
            el::Configurations loggingConfig;
            loggingConfig.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
          }
          //json["loggers"].is_array();
          //std::string value_1 = json["log"];
          //std::cout << "value_1: '" + value_1 + "'" << std::endl;
        }
      }
    }
  } catch (...) {
    std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"));
  }
}
