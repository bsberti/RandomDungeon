// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "Leaderboard.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class LeaderboardHandler : virtual public LeaderboardIf {
 public:
  LeaderboardHandler() {
    // Your initialization goes here
  }

  void setHighScore(const int32_t playerId, const int32_t highScore) {
    // Your implementation goes here
    printf("setHighScore\n");
  }

  void getTop20(std::map<int32_t, int32_t> & _return) {
    // Your implementation goes here
    printf("getTop20\n");
  }

  bool login(const std::string& email, const std::string& password, const bool newLogin) {
    // Your implementation goes here
    printf("login\n");
  }

  bool createAccount(const std::string& email, const std::string& hashedPassword, const std::string& salt) {
    // Your implementation goes here
    printf("createAccount\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<LeaderboardHandler> handler(new LeaderboardHandler());
  shared_ptr<TProcessor> processor(new LeaderboardProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

