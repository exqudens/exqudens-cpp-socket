#pragma once

#include <string>
#include <vector>

#define ELPP_NO_DEFAULT_LOG_FILE
#include <easylogging++.h>

class TestLogging {

  friend class TestApplication;

  public:

    static std::string defaultConfig();

    static std::string defaultGlobalConfig();

  private:

    static void config(const std::string& file, const std::string& workingDir = "");

    static std::string config(const std::vector<std::string>& commandLineArgs);

};
