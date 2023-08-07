#pragma once

#include <cstddef>
#include <chrono>
#include <vector>
#include <exception>
#include <thread>
#include <mutex>
#include <iostream>
#include <format>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "TestThreadPool.hpp"
#include "exqudens/SocketServer.hpp"
#include "exqudens/SocketClient.hpp"

namespace exqudens::socket {

  class Tests: public testing::Test {

    private:

      std::mutex mutex = {};

    public:

      void serverLog(const std::string& message) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::cout << std::format("[server] {}", message) << std::endl;
      }

      void clientLog(const std::string& message) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::cout << std::format("[client] {}", message) << std::endl;
      }

      std::vector<char> exchange(const std::vector<char>& in) {
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
      unsigned short port = 27015;
      SocketServer server;
      server.setPort(port);
      server.setLogHandler(&Tests::serverLog, this);
      server.setExchangeHandler(&Tests::exchange, this);
      SocketClient client;
      client.setPort(port);
      client.setLogHandler(&Tests::clientLog, this);

      std::future<void> future = pool.submit(&SocketServer::runOnce, &server);

      std::string data = "Abc123!";
      std::vector<char> bytes(data.begin(), data.end());

      size_t expected = bytes.size();
      std::vector<char> outputBuffer = client.exchange(bytes);
      size_t actual = 0;
      std::memcpy(&actual, outputBuffer.data(), sizeof(size_t));

      std::cout << std::format("expected: '{}'", expected) << std::endl;
      std::cout << std::format("actual: '{}'", actual) << std::endl;

      ASSERT_EQ(expected, actual);

      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  /*TEST_F(Tests, test2) {
    try {
      TestThreadPool pool(1, 1);
      unsigned short port = 27015;
      SocketServer server;
      server.setPort(port);
      server.setLogHandler(&Tests::staticServerLog);
      server.setExchangeHandler([this](const std::vector<char>& in) { return accept(in); });
      SocketClient client = SocketClient();
      client.setPort(port);
      client.setLogHandler(&Tests::staticClientLog);

      std::future<void> future = pool.submit(&SocketServer::run, &server);

      std::string data = "Abc123!";
      std::vector<char> bytes(data.begin(), data.end());

      size_t expected = bytes.size();
      std::vector<char> outputBuffer = client.exchange(bytes);
      size_t actual = 0;
      std::memcpy(&actual, outputBuffer.data(), sizeof(size_t));

      RecordProperty("expected", std::to_string(expected));
      RecordProperty("actual", std::to_string(actual));
      std::cout << std::format("expected: '{}'", expected) << std::endl;
      std::cout << std::format("actual: '{}'", actual) << std::endl;

      ASSERT_EQ(expected, actual);

      expected = bytes.size();
      outputBuffer = client.exchange(bytes);
      actual = 0;
      std::memcpy(&actual, outputBuffer.data(), sizeof(size_t));

      RecordProperty("expected", std::to_string(expected));
      RecordProperty("actual", std::to_string(actual));
      std::cout << std::format("expected: '{}'", expected) << std::endl;
      std::cout << std::format("actual: '{}'", actual) << std::endl;

      ASSERT_EQ(expected, actual);

      server.stop();
      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }*/

  /*TEST_F(Tests, test3) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server;
      server.setLogHandler(&Tests::staticServerLog);
      server.setExchangeHandler(&Tests::accept, this);

      std::future<void> future = pool.submit(&SocketServer::run, &server);

      SocketClient client = SocketClient();
      client.setLogHandler(&Tests::staticClientLog);

      std::string data = std::string(1024, 'a') + std::string(1024, '1');
      std::vector<char> bytes(data.begin(), data.end());

      std::cout << std::format("bytes.size: '{}'", bytes.size()) << std::endl;

      size_t expected = bytes.size();
      std::vector<char> outputBuffer = client.exchange(bytes);
      size_t actual = 0;
      std::memcpy(&actual, outputBuffer.data(), sizeof(size_t));

      RecordProperty("expected", std::to_string(expected));
      RecordProperty("actual", std::to_string(actual));
      std::cout << std::format("expected: '{}'", expected) << std::endl;
      std::cout << std::format("actual: '{}'", actual) << std::endl;

      ASSERT_EQ(expected, actual);

      server.stop();
      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }*/

  /*TEST_F(Tests, test4) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server;
      server.setLogHandler(&Tests::staticServerLog);

      std::future<void> future = pool.submit(&SocketServer::runOnce, &server);

      std::this_thread::sleep_for(std::chrono::seconds(5));

      server.stop();
      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }*/

}
