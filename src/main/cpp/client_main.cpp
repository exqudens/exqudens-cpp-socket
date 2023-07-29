#include "exqudens/SocketClient.hpp"

#include <string>
#include <vector>

int main(int argc, char** argv) {
  exqudens::SocketClient client;
  //client.setPort(12345);

  std::string data = "Abc123!";
  std::vector<char> bytes(data.begin(), data.end());

  size_t expected = bytes.size();
  size_t actual = client.sendData(bytes);

  return 0;
}
