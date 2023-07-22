#include "TestApplication.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "exqudens/Tests.hpp"

int TestApplication::run(int* argc, char** argv) {
  testing::InitGoogleMock(argc, argv);
  testing::InitGoogleTest(argc, argv);
  return RUN_ALL_TESTS();
}
