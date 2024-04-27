#include <cstddef>
#include <cctype>
#include <stdexcept>
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <fstream>

#include <nlohmann/json.hpp>

#include "TestUtils.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

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
      previous.emplace_back(CALL_INFO + ": Empty stack!");
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
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
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::ltrim(const std::string& value) {
  try {
    std::string str = value;
    str.erase(
        str.begin(),
        std::find_if(str.begin(), str.end(), [](char c) {return !std::isspace(c);})
    );
    return str;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::rtrim(const std::string& value) {
  try {
    std::string str = value;
    str.erase(
        std::find_if(str.rbegin(), str.rend(), [](char c) {return !std::isspace(c);}).base(),
        str.end()
    );
    return str;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::trim(const std::string& value) {
  try {
    std::string str = value;
    str = ltrim(str);
    str = rtrim(str);
    return str;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::toUpper(const std::string& value) {
  try {
    std::string str = value;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::toLower(const std::string& value) {
  try {
    std::string str = value;
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::replaceAll(const std::string& value, const std::string& target, const std::string& replacement) {
  try {
    std::string str = value;
    if (target == replacement) {
      return str;
    }
    std::size_t foundAt = std::string::npos;
    while ((foundAt = str.find(target, foundAt + 1)) != std::string::npos) {
      str.replace(foundAt, target.length(), replacement);
    }
    return str;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::vector<char> TestUtils::readFileBytes(const std::string& path) {
  try {
    std::filesystem::path filePath = std::filesystem::path(path).make_preferred();
    std::size_t fileSizeRaw = std::filesystem::file_size(filePath);

    if (fileSizeRaw > LLONG_MAX) {
      throw std::runtime_error(CALL_INFO + ": File size is larger than read max size: '" + std::to_string(fileSizeRaw) + " > " + std::to_string(LLONG_MAX) + "'!");
    }

    auto fileSize = (std::streamsize) fileSizeRaw;
    std::vector<char> buffer(fileSize);
    std::ifstream inputStream(filePath.string(), std::ios::binary);

    if (!inputStream.is_open()) {
      throw std::runtime_error(CALL_INFO + ": Failed to open file: '" + path + "'!");
    }

    inputStream.read(buffer.data(), fileSize);
    inputStream.close();

    return buffer;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::readFileString(const std::string& path) {
  try {
    std::vector<char> bytes = readFileBytes(path);
    std::string result(bytes.begin(), bytes.end());

    return result;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::string TestUtils::join(const std::vector<std::string>& value, const std::string& delimiter, const std::string& prefix, const std::string& suffix) {
  try {
    std::string result;
    if (value.empty()) {
      return result;
    }
    result = std::accumulate(
        value.begin(),
        value.end(),
        std::string(),
        [&delimiter](const std::string& a, const std::string& b) {
          return a + (a.empty() ? "" : delimiter) + b;
        }
    );
    result = prefix + result + suffix;
    return result;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

std::any TestUtils::nlohmannJsonToAny(const std::any& value) {
  try {
    if (typeid(nlohmann::json) != value.type()) {
      throw std::runtime_error(CALL_INFO + ": Unsupported type: '" + value.type().name() + "' supported type: '" + typeid(nlohmann::json).name() + "'");
    }

    nlohmann::json nlohmannJson = std::any_cast<nlohmann::json>(value);

    if (nlohmannJson.is_object()) {
      std::map<std::string, std::any> container;
      for (const auto& entry : nlohmannJson.items()) {
        std::string k = entry.key();
        std::any v = nlohmannJsonToAny(entry.value());
        container.insert({k, v});
      }
      std::any result = container;
      return result;
    } else if (nlohmannJson.is_array()) {
      std::vector<std::any> container;
      for (size_t i = 0; i < nlohmannJson.size(); i++) {
        std::any r = nlohmannJsonToAny(nlohmannJson.at(i));
        container.emplace_back(r);
      }
      std::any result = container;
      return result;
    } else {
      if (nlohmannJson.is_boolean()) {
        std::any result = nlohmannJson.get<bool>();
        return result;
      } else if (nlohmannJson.is_number()) {
        if (nlohmannJson.is_number_float()) {
          std::any result = nlohmannJson.get<double>();
          return result;
        } else if (nlohmannJson.is_number_integer()) {
          std::any result = nlohmannJson.get<long>();
          return result;
        } else if (nlohmannJson.is_number_unsigned()) {
          std::any result = nlohmannJson.get<unsigned long>();
          return result;
        }
      } else if (nlohmannJson.is_string()) {
        std::any result = nlohmannJson.get<std::string>();
        return result;
      }
    }

    std::any result = nullptr;
    return result;
  } catch (...) {
    std::throw_with_nested(std::runtime_error(CALL_INFO));
  }
}

#undef CALL_INFO
