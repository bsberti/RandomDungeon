#include "Leaderboard.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "..\MyGameServer\SQLiteDBHelper.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport; 
using namespace ::apache::thrift::server;

using boost::shared_ptr;
SQLiteDBHelper* SQLiteHelper;

class LeaderboardHandler : virtual public LeaderboardIf {
public:
	LeaderboardHandler() {
		// Your initialization goes here
		this->highScoreByPlayer = new std::map<int32_t, int32_t>();
		SQLiteHelper = new SQLiteDBHelper();
		SQLiteHelper->ConnectToDB("RandomDungeon");
	}

	void setHighScore(const int32_t playerId, const int32_t highScore) {
		this->highScoreByPlayer->insert(std::pair<int32_t, int32_t>(playerId, highScore));

		// Your implementation goes here
		printf("setHighScore\n");
	}

	void getTop20(std::map<int32_t, int32_t> & _return) {
		SQLiteHelper->ExecuteQuery("SELECT * FROM leaderboard ORDER BY highScore DESC LIMIT 20;");

		for (int i = 0; i < SQLiteHelper->vec_SQLiteResult->size(); i++) {
			//printf(SQLiteHelper->vec_SQLiteResult->at(i).userId);
			int32_t userID = std::stoi((*SQLiteHelper->vec_SQLiteResult)[i].userId);
			int32_t highScore = std::stoi((*SQLiteHelper->vec_SQLiteResult)[i].highScore);
			//_return[userID] = highScore;
			_return.insert(std::make_pair(userID, highScore));
		}

		//_return.insert(this->highScoreByPlayer->begin(), this->highScoreByPlayer->end());
		// Your implementation goes here
		printf("getTop20\n");
	}
private:
	std::map<int32_t, int32_t> *highScoreByPlayer;
};

int main(int argc, char **argv) {
	WSAData wsaData;
	int initializationResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initializationResult != 0) {
		printf("Failed to init winsock %d\n", initializationResult);
		return 1;
	}

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

