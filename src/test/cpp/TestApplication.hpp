#pragma once

// resolve error C2011: 'sockaddr': 'struct' type redefinition
#include "exqudens/socket/SocketFactory.hpp"

class TestApplication {

  private:

    inline static const char* LOGGER_ID = "TestApplication";

  public:

    static int run(int argc, char** argv);

};
