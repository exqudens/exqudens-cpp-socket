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

      static std::vector<char> accept1(const std::vector<char>& in) {
        size_t inSize = in.size();
        std::cout << std::format("inSize: '{}'", inSize) << std::endl;
        std::vector<char> out(sizeof(inSize));
        std::copy(
            static_cast<const char*>(static_cast<const void*>(&inSize)),
            static_cast<const char*>(static_cast<const void*>(&inSize)) + sizeof(inSize),
            out.data()
        );
        return out;
      }

      std::vector<char> accept2(const std::vector<char>& in) {
        size_t inSize = in.size();
        std::cout << std::format("inSize: '{}'", inSize) << std::endl;
        std::vector<char> out(sizeof(inSize));
        std::copy(
            static_cast<const char*>(static_cast<const void*>(&inSize)),
            static_cast<const char*>(static_cast<const void*>(&inSize)) + sizeof(inSize),
            out.data()
        );
        return out;
      }

  };

  TEST_F(Tests, test1) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server = SocketServer();
      //server.setReceiveHandler(&Tests::accept1);
      //server.setReceiveHandler([this](const std::vector<char>& in) { return accept2(in); });
      server.setReceiveHandler(&Tests::accept2, this);
      SocketClient client = SocketClient();

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

      server.stop();
      future.get();
    } catch (const std::exception& e) {
      FAIL() << e.what();
    }
  }

}
