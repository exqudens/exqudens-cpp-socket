#pragma once

#include <vector>
#include <exception>
#include <iostream>
#include <format>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestThreadPool.hpp"
#include "exqudens/SocketServer.hpp"

namespace exqudens::socket {

  class Tests: public testing::Test {

    protected:

      static void accept(const std::vector<char>& data) {
        //
      }

  };

  TEST_F(Tests, test1) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server = SocketServer().setPort(8080).setReceiveFunction(&Tests::accept);

      pool.submit(&SocketServer::start, &server);

      std::cout << std::format("AAA") << std::endl;
    } catch (const std::exception& e) {
      FAIL() << e.what();
    }
  }

}
