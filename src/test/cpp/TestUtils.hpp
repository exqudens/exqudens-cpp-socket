#pragma once

#include <string>
#include <any>
#include <vector>
#include <exception>

class TestUtils {

  public:

    static std::vector<std::string> toStringVector(const std::exception& exception, std::vector<std::string> previous = {});

    static std::vector<std::string> toStackTrace(const std::exception& exception);

    static std::string toString(const std::exception& exception);

    static std::string ltrim(const std::string& value);

    static std::string rtrim(const std::string& value);

    static std::string trim(const std::string& value);

    static std::string toUpper(const std::string& value);

    static std::string toLower(const std::string& value);

    static std::string replaceAll(const std::string& value, const std::string& target, const std::string& replacement);

    static std::vector<char> readFileBytes(const std::string& path);

    static std::string readFileString(const std::string& path);

    static std::string join(const std::vector<std::string>& value, const std::string& delimiter, const std::string& prefix = "", const std::string& suffix = "");

    static std::any nlohmannJsonToAny(const std::any& value);

};
