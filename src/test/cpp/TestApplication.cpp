#include <cstddef>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestApplication.hpp"
#include "TestConfiguration.hpp"
#include "TestLogging.hpp"
#include "TestUtils.hpp"

// include test files
#include "exqudens/Tests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGING_CONFIG std::string("--logging-config")

int TestApplication::run(int argc, char** argv) {
  try {
    if (argc <= 0) {
      throw std::runtime_error(CALL_INFO + ": argc <= 0");
    }

    std::vector<std::string> args = {};
    for (int i = 0; i < argc; i++) {
      args.emplace_back(argv[i]);
    }

    TestConfiguration::setExecutableFile(args.front());

    if (TestConfiguration::getExecutableDir().empty()) {
      throw std::runtime_error(CALL_INFO + ": TestConfiguration::getExecutableDir().empty()");
    }

    if (TestConfiguration::getExecutableFile().empty()) {
      throw std::runtime_error(CALL_INFO + ": TestConfiguration::getExecutableFile().empty()");
    }

    std::optional<std::string> loggingConfigFile = {};
    std::string configType;

    // try command line args
    for (size_t i = 0; i < args.size(); i++) {
      std::cout << "args[" << i << "]: '" << args.at(i) << "'" << std::endl;
      if (LOGGING_CONFIG == args.at(i) && (i + 1) <= (args.size() - 1)) {
        std::filesystem::path loggingFile(args.at(i + 1));
        if (std::filesystem::exists(loggingFile)) {
          loggingConfigFile = loggingFile.generic_string();
          configType = "command-line-arg (file: '" + loggingFile.generic_string() + "')";
        }
      }
    }
    // try executable dir
    if (!loggingConfigFile.has_value()) {
      std::filesystem::path executableDir = std::filesystem::path(args.front()).parent_path();
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
      throw std::runtime_error(CALL_INFO + ": Can't find: 'logging-config.txt'");
    } else {
      TestLogging::config(loggingConfigFile.value(), std::filesystem::path(args.front()).parent_path().generic_string());
      configType = "default";
    }

    LOG(INFO) << "Logging config type: '" << configType << "'";

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
  } catch (const std::exception& e) {
    std::cerr << CALL_INFO + ": " + TestUtils::toString(e) << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << CALL_INFO + ": Unknown error!" << std::endl;
    return EXIT_FAILURE;
  }
}

#undef CALL_INFO
#undef LOGGING_CONFIG
