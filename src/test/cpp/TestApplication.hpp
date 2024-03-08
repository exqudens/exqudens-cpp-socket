#pragma once

// resolve error C2011: 'sockaddr': 'struct' type redefinition
#include "exqudens/socket/SocketFactory.hpp"

class TestApplication {

  public:

    static int run(int argc, char** argv);

};
