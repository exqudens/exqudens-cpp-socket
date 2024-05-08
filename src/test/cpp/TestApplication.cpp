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
#include "exqudens/SocketTests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

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

    std::string configType = TestLogging::config(args);

    TEST_LOGGING_INFO(LOGGER_ID) << "Logging config type: '" << configType << "'";

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
