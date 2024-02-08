#pragma once

#include <string>

#include <easylogging++.h>

#ifndef DEFAULT_LOGGER
#  define DEFAULT_LOGGER(level) LOG(level)
#endif /* DEFAULT_LOGGER */

#ifndef LOGGER
#  define LOGGER(level, id) if (el::Loggers::getLogger(id) != nullptr) CLOG(level, id)
#endif /* LOGGER */

class TestLogging {

  friend class TestApplication;

  private:

    static void config(const std::string& value);

};
