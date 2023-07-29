#include "exqudens/SocketServer.hpp"

int main(int argc, char** argv) {
  exqudens::SocketServer server;
  //server.setPort(12345);
  server.setReceiveFunction(
      [](const std::vector<char>& in) {
        size_t inSize = in.size();
        std::vector<char> out(sizeof(inSize));
        std::copy(
            static_cast<const char*>(static_cast<const void*>(&inSize)),
            static_cast<const char*>(static_cast<const void*>(&inSize)) + sizeof(inSize),
            out.data()
        );
        return out;
      }
  );
  server.start();
  return 0;
}
