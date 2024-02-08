#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestApplication.hpp"
#include "TestLogging.hpp"
#include "TestUtils.hpp"
#include "exqudens/Tests.hpp"

#define LOGGING_CONFIG std::string("--logging-config")

int TestApplication::run(int argc, char** argv) {
  try {
    std::vector<std::string> args = {};
    for (int i = 0; i < argc; i++) {
      args.emplace_back(argv[i]);
    }

    for (size_t i = 0; i < args.size(); i++) {
      std::cout << "args[" << i << "]: '" << args.at(i) << "'" << std::endl;
      if (LOGGING_CONFIG == args.at(i) && (i + 1) <= (args.size() - 1)) {
        TestLogging::config(std::filesystem::path(args.at(i + 1)).generic_string());
      }
    }

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  } catch (const std::exception& e) {
    std::cerr << TestUtils::toString(e) << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + "): Unknown error!" << std::endl;
    return EXIT_FAILURE;
  }
}

#undef LOGGING_CONFIG
