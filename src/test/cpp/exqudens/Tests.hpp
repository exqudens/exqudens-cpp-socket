#pragma once

#include <cstddef>
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

    public:

      std::vector<char> accept(const std::vector<char>& data) {
        size_t dataSize = data.size();
        std::vector<char> result(sizeof(dataSize));
        std::copy(
            static_cast<const char*>(static_cast<const void*>(&dataSize)),
            static_cast<const char*>(static_cast<const void*>(&dataSize)) + sizeof(dataSize),
            result.data()
        );
        return result;
      }

  };

  TEST_F(Tests, test1) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server = SocketServer().setPort(8080).setReceiveFunction(std::bind(&Tests::accept, this, std::placeholders::_1));
      SocketClient client = SocketClient().setPort(8080);

      std::future<void> future = pool.submit(&SocketServer::start, &server);

      std::string data = "Abc123!";
      std::vector<char> bytes(data.begin(), data.end());

      size_t expected = bytes.size();
      size_t actual = client.sendData(bytes);

      RecordProperty("expected", std::to_string(expected));
      RecordProperty("actual", std::to_string(actual));
      std::cout << std::format("expected: '{}'", expected) << std::endl;
      std::cout << std::format("actual: '{}'", actual) << std::endl;

      ASSERT_EQ(expected, actual);

      future.get();
    } catch (const std::exception& e) {
      FAIL() << e.what();
    }
  }

}
