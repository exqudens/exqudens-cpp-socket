#pragma once

#include <vector>
#include <exception>
#include <iostream>
#include <format>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestThreadPool.hpp"
#include "exqudens/SocketServer.hpp"
#include "exqudens/SocketClient.hpp"

namespace exqudens::socket {

  class Tests: public testing::Test {

    protected:

      void accept(const std::vector<char>& data) {
        std::cout << std::format("Tests::accept(data.size: {})", data.size()) << std::endl;
      }

  };

  TEST_F(Tests, test1) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server = SocketServer().setPort(8080).setReceiveFunction(
          [this](auto&& v) {
            accept(std::forward<decltype(v)>(v));
          }
      );
      SocketClient client = SocketClient().setPort(8080);

      pool.submit(&SocketServer::start, &server);

      int actual = client.sendData({'A', 'b', 'c', '1', '2', '3', '!'});

      std::cout << std::format("AAA: '{}'", actual) << std::endl;
    } catch (const std::exception& e) {
      FAIL() << e.what();
    }
  }

}
