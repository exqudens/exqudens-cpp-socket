#include <cstdlib>
#include <stdexcept>
#include <filesystem>

#include "TestConfiguration.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

std::string TestConfiguration::getExecutableFile() {
  try {
    return executableFile.value();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestConfiguration::getExecutableDir() {
  try {
    return executableDir.value();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

void TestConfiguration::setExecutableFile(const std::string& value) {
  try {
    std::filesystem::path path = std::filesystem::path(value).make_preferred();
    TestConfiguration::executableFile = path.string();
    TestConfiguration::executableDir = path.parent_path().string();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

#undef CALL_INFO
