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
      std::vector<size_t> receivedSizes = {};

    public:

      void serverLog(const std::string& message) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::cout << std::format("[server] {}", message) << std::endl;
      }

      void clientLog(const std::string& message) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::cout << std::format("[client] {}", message) << std::endl;
      }

      void receive(const std::vector<char>& value) {
        std::cout << std::format("receive.size: '{}'", value.size()) << std::endl;
        receivedSizes.emplace_back(value.size());
      }

      std::vector<char> send() {
        std::vector<char> result = {};
        if (!receivedSizes.empty()) {
          size_t size = receivedSizes.front();
          result = std::vector<char>(sizeof(size_t));
          std::copy(
              static_cast<const char*>(static_cast<const void*>(&size)),
              static_cast<const char*>(static_cast<const void*>(&size)) + sizeof(size),
              result.data()
          );
          receivedSizes.erase(receivedSizes.begin());
        }
        std::cout << std::format("send.size: '{}'", result.size()) << std::endl;
        return result;
      }

      std::vector<size_t> bytesToSizes(const std::vector<char>& value) {
        std::vector<size_t> sizes;
        for (size_t i = 0; i < value.size(); i += sizeof(size_t)) {
          std::vector<char> tmp;
          for (size_t j = 0; j < sizeof(size_t); j++) {
            tmp.emplace_back(value.at(i + j));
          }
          size_t v = 0;
          std::memcpy(&v, tmp.data(), sizeof(size_t));
          sizes.emplace_back(v);
        }
        return sizes;
      }

    protected:

      void SetUp() override {
        receivedSizes.clear();
      }

  };

  TEST_F(Tests, test1) {
    try {
      TestThreadPool pool(2, 2);

      SocketServer server;
      server.setLogHandler(&Tests::serverLog, this);
      server.setReceiveHandler(&Tests::receive, this);
      server.setSendHandler(&Tests::send, this);

      std::future<void> runOnceFuture = pool.submit(&SocketServer::runOnce, &server);

      std::this_thread::sleep_for(std::chrono::seconds(3));

      std::future<void> stopFuture = pool.submit(&SocketServer::stop, &server);

      stopFuture.get();
      runOnceFuture.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(Tests, test2) {
    try {
      TestThreadPool pool(1, 1);
      unsigned short port = 27015;
      SocketServer server;
      server.setPort(port);
      server.setLogHandler(&Tests::serverLog, this);
      server.setReceiveHandler(&Tests::receive, this);
      server.setSendHandler(&Tests::send, this);
      SocketClient client;
      client.setPort(port);
      client.setLogHandler(&Tests::clientLog, this);

      std::future<void> future = pool.submit(&SocketServer::runOnce, &server);

      std::string data = std::string(1024, 'a') + std::string(1024, '1') + "ABC";
      std::vector<char> bytes = std::vector<char>(data.begin(), data.end());

      bytes = client.exchange(bytes);

      std::vector<size_t> sizes = bytesToSizes(bytes);

      std::cout << std::format("sizes.size: '{}'", sizes.size()) << std::endl;
      std::cout << std::format("sizes[0]: '{}'", sizes.at(0)) << std::endl;
      std::cout << std::format("sizes[1]: '{}'", sizes.at(1)) << std::endl;
      std::cout << std::format("sizes[2]: '{}'", sizes.at(2)) << std::endl;

      ASSERT_EQ(3, sizes.size());
      ASSERT_EQ(1024, sizes.at(0));
      ASSERT_EQ(1024, sizes.at(1));
      ASSERT_EQ(3, sizes.at(2));

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
