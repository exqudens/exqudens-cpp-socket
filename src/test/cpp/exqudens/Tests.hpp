#pragma once

#include <cstddef>
#include <cstring>
#include <chrono>
#include <string>
#include <vector>
#include <exception>
#include <thread>
#include <mutex>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "TestThreadPool.hpp"
#include "exqudens/Sockets.hpp"

namespace exqudens::socket {

  class Tests: public testing::Test {

    private:

      inline static std::mutex mutex = {};

    public:

      static void socketsLog(const std::string& message) {
        const std::lock_guard<std::mutex> lock(mutex);
        std::cout << "[SOCKETS] " << message << std::endl;
      }

      static std::vector<char> toBytes(const size_t& value) {
        size_t sizeOfValue = sizeof(value);
        std::vector<char> result = std::vector<char>(sizeOfValue);
        std::copy(
            static_cast<const char*>(static_cast<const void*>(&value)),
            static_cast<const char*>(static_cast<const void*>(&value)) + sizeOfValue,
            result.data()
        );
        return result;
      }

      static size_t toSize(const std::vector<char>& value) {
        size_t result = 0;
        std::memcpy(&result, value.data(), sizeof(result));
        return result;
      }

      static std::vector<std::vector<char>> split(const std::vector<char>& value, const size_t& size) {
        std::vector<std::vector<char>> result = {};
        for (size_t i = 0; i < value.size(); i += size) {
          std::vector<char> part = {};
          for (size_t j = 0; j < size && i + j < value.size(); j++) {
            part.emplace_back(value.at(i + j));
          }
          result.emplace_back(part);
        }
        return result;
      }

    protected:

      static void SetUpTestSuite() {
        Sockets::setLogHandler(&Tests::socketsLog);
        Sockets::init();
      }

      static void TearDownTestSuite() {
        Sockets::destroy();
      }

  };

  TEST_F(Tests, test1) {
    try {
      TestThreadPool pool(2, 2);

      SocketServer server = Sockets::createServer();

      std::future<void> future = pool.submit(&SocketServer::init, &server);

      std::this_thread::sleep_for(std::chrono::seconds(3));

      server.destroy();

      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(Tests, test2) {
    try {
      TestThreadPool pool(1, 1);
      SocketServer server = Sockets::createServer();
      SocketClient client = Sockets::createClient();

      std::future<void> future = pool.submit([&server]() {
        server.init();

        std::vector<char> receivedData;
        std::vector<char> buffer;

        buffer = server.receiveData();
        server.sendData(Tests::toBytes(buffer.size()));
        receivedData.insert(receivedData.end(), buffer.begin(), buffer.end());

        buffer = server.receiveData();
        server.sendData(Tests::toBytes(buffer.size()));
        receivedData.insert(receivedData.end(), buffer.begin(), buffer.end());

        buffer = server.receiveData();
        server.sendData(Tests::toBytes(buffer.size()));
        receivedData.insert(receivedData.end(), buffer.begin(), buffer.end());

        server.destroy();
      });

      std::string data = std::string(1024, 'a') + std::string(1024, '1') + "ABC";
      std::vector<char> buffer = std::vector<char>(data.begin(), data.end());
      std::vector<std::vector<char>> parts = Tests::split(buffer, 1024);
      size_t size = 0;
      std::vector<size_t> sizes = {};

      std::vector<char> receivedData;

      client.init();

      client.sendData(parts.at(0));
      buffer = client.receiveData();
      size = Tests::toSize(buffer);
      sizes.emplace_back(size);

      client.sendData(parts.at(1));
      buffer = client.receiveData();
      size = Tests::toSize(buffer);
      sizes.emplace_back(size);

      client.sendData(parts.at(2));
      buffer = client.receiveData();
      size = Tests::toSize(buffer);
      sizes.emplace_back(size);

      client.destroy();

      ASSERT_EQ(3, sizes.size());
      ASSERT_EQ(1024, sizes.at(0));
      ASSERT_EQ(1024, sizes.at(1));
      ASSERT_EQ(3, sizes.at(2));

      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

}
