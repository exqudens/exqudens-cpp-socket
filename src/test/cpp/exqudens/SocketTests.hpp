#pragma once

#include <cstddef>
#include <cstring>
#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <exception>
#include <thread>
#include <mutex>
#include <filesystem>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestLogging.hpp"
#include "TestUtils.hpp"
#include "TestThreadPool.hpp"
#include "exqudens/socket/SocketFactory.hpp"

namespace exqudens {

  class SocketTests: public testing::Test {

    protected:

      inline static const char* LOGGER_ID = "exqudens.SocketTests";

    private:

      inline static std::mutex mutex = {};

    public:

      static void socketsLog(const std::string& message, const unsigned short& level, const std::string& function, const std::string& file, const size_t& line) {
        const std::lock_guard<std::mutex> lock(mutex);
        CLOG(INFO, LOGGER_ID) << "[" + function + " " + "(" + file + ":" + std::to_string(line) + ")" + "] " << message;
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

  };

  TEST_F(SocketTests, test1) {
    try {
      SocketFactory socketFactory;
      std::string message;

      try {
        socketFactory.createSharedServer();
      } catch (const std::exception& exception) {
        message = TestUtils::toString(exception);
      }

      CLOG(INFO, LOGGER_ID) << message;

      ASSERT_FALSE(message.empty());
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(SocketTests, test2) {
    try {
      TestThreadPool pool(2, 2);
      SocketFactory socketFactory;

      socketFactory.setLogFunction(&SocketTests::socketsLog);
      socketFactory.init();
      std::unique_ptr<ISocket> server = socketFactory.createUniqueServer();

      std::future<void> future = pool.submit(&ISocket::init, server.get());

      std::this_thread::sleep_for(std::chrono::seconds(3));

      server->destroy();
      socketFactory.destroy();

      future.get();
    } catch (const std::exception& e) {
      FAIL() << TestUtils::toString(e);
    }
  }

  TEST_F(SocketTests, test3) {
    try {
      TestThreadPool pool(1, 1);
      SocketFactory socketFactory;

      socketFactory.setLogFunction(&SocketTests::socketsLog);
      socketFactory.init();
      std::shared_ptr<ISocket> server = socketFactory.createSharedServer();
      server->setPort(27015);
      std::shared_ptr<ISocket> client = socketFactory.createSharedClient();
      client->setPort(27015);
      client->setHost("localhost");

      std::future<void> future = pool.submit([&server]() {
        server->init();

        std::vector<char> receivedData;
        std::vector<char> buffer;

        buffer = server->receiveData();
        server->sendData(SocketTests::toBytes(buffer.size()));
        receivedData.insert(receivedData.end(), buffer.begin(), buffer.end());

        buffer = server->receiveData();
        server->sendData(SocketTests::toBytes(buffer.size()));
        receivedData.insert(receivedData.end(), buffer.begin(), buffer.end());

        buffer = server->receiveData();
        server->sendData(SocketTests::toBytes(buffer.size()));
        receivedData.insert(receivedData.end(), buffer.begin(), buffer.end());

        server->destroy();
      });

      std::string data = std::string(1024, 'a') + std::string(1024, '1') + "ABC";
      std::vector<char> buffer = std::vector<char>(data.begin(), data.end());
      std::vector<std::vector<char>> parts = SocketTests::split(buffer, 1024);
      size_t size = 0;
      std::vector<size_t> sizes = {};

      std::vector<char> receivedData;

      client->init();

      client->sendData(parts.at(0));
      buffer = client->receiveData();
      size = SocketTests::toSize(buffer);
      sizes.emplace_back(size);

      client->sendData(parts.at(1));
      buffer = client->receiveData();
      size = SocketTests::toSize(buffer);
      sizes.emplace_back(size);

      client->sendData(parts.at(2));
      buffer = client->receiveData();
      size = SocketTests::toSize(buffer);
      sizes.emplace_back(size);

      client->destroy();
      socketFactory.destroy();

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
