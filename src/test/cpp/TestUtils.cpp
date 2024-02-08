#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <sstream>

#include "TestUtils.hpp"

std::vector<std::string> TestUtils::toStringVector(
    const std::exception& exception,
    std::vector<std::string> previous
) {
  previous.emplace_back(exception.what());
  try {
    std::rethrow_if_nested(exception);
    return previous;
  } catch (const std::exception& e) {
    return toStringVector(e, previous);
  } catch (...) {
    if (previous.empty()) {
      previous.emplace_back(std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + "): Empty stack!");
    }
    return previous;
  }
}

std::vector<std::string> TestUtils::toStackTrace(const std::exception& exception) {
  try {
    std::vector<std::string> elements = toStringVector(exception);
    if (elements.size() > 1) {
      std::ranges::reverse(elements);
    }
    return elements;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"));
  }
}

std::string TestUtils::toString(const std::exception& exception) {
  try {
    std::vector<std::string> stackTrace = toStackTrace(exception);
    std::ostringstream out;
    for (size_t i = 0; i < stackTrace.size(); i++) {
      out << stackTrace[i];
      if (i < stackTrace.size() - 1) {
        out << std::endl;
      }
    }
    return out.str();
  } catch (...) {
    std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"));
  }
}
