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
		//this->highScoreByPlayer->insert(std::pair<int32_t, int32_t>(playerId, highScore));

		// Your implementation goes here
		printf("Setting HighScore:\n");
		std::string query = "SELECT * FROM leaderboard WHERE userID = " + std::to_string(playerId);

		SQLiteHelper->ExecuteQuery(query.c_str());

		if (SQLiteHelper->vec_SQLiteResult->size() != 0) {
			//for (int i = 0; i < SQLiteHelper->vec_SQLiteResult->size(); i++) {
			//	//printf(SQLiteHelper->vec_SQLiteResult->at(i).userId);
			//	int32_t userID = std::stoi((*SQLiteHelper->vec_SQLiteResult)[i].userId);
			//	int32_t highScore = std::stoi((*SQLiteHelper->vec_SQLiteResult)[i].highScore);
			//	//_return[userID] = highScore;
			//}
			int32_t userID = std::stoi((*SQLiteHelper->vec_SQLiteResult)[0].userId);
			int32_t highScoreReturned = std::stoi((*SQLiteHelper->vec_SQLiteResult)[0].highScore);

			printf("userID: %d founded.\n", userID);

			if (highScoreReturned < highScore) {
				// UPDATE
				query = "UPDATE leaderboard SET highScore = " + std::to_string(highScore)
					+ " WHERE userID = " + std::to_string(playerId) + ";";

				SQLiteHelper->ExecuteQuery(query.c_str());

				if (SQLiteHelper->vec_SQLiteResult->size() != 0) {
					int breakPoint = 5;
					// ERROR?
					printf("ERROR\n");
				}
				else {
					printf("userID: %d UPDATED.\n", userID);
				}
			}
			else {
				printf("userID: %d NOT UPDATED.\n New HighScore (%d) < Old HighScore (%d) \n", userID, highScoreReturned, highScore);
			}

		}
		else {
			// INSERT
			printf("userID NOT founded.\n");
			query = "INSERT INTO leaderboard VALUES (" + std::to_string(playerId)
				+ ", " + std::to_string(highScore) + ");";

			SQLiteHelper->ExecuteQuery(query.c_str());

			if (SQLiteHelper->vec_SQLiteResult->size() != 0) {
				int breakPoint = 5;
				// ERROR?
			}
			else {
				printf("userID: %d UPDATED.\n", playerId);
			}
		}
	}

	void getTop20(std::map<int32_t, int32_t>& _return) {
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

	bool login(const std::string& email, const std::string& password, bool& newLogin) {
		bool isNewLogin = false;

		if (SQLiteHelper->Login(email, password, isNewLogin)) {

			newLogin = isNewLogin;
			return true;
		}

		return false;
	}

	bool createAccount(const std::string& email, const std::string& hashedPassword, const std::string& salt) {
		return SQLiteHelper->CreateAccount(email, hashedPassword, salt);
	}

	void getUserProperties(UserProperties& _return, const int32_t playerId) {
		userPropertiesResultSet resultSet;
		std::string playerID = std::to_string(playerId);
		if (SQLiteHelper->GetUserProperties(resultSet, playerID)) {
			_return.userID		= resultSet.userID;
			_return.date		= atoi(resultSet.date.c_str());
			_return.strengh		= resultSet.strengh;
			_return.magicPower	= resultSet.magicPower;
			_return.agility		= resultSet.agility;
			_return.maxHealth	= resultSet.max_health;
			_return.maxMana		= resultSet.max_mana;
			_return.villager	= resultSet.villager;
			_return.level		= resultSet.level;
		}
	}

	void setUserProperties(const UserProperties& userPropertiesPacket) {
		userPropertiesResultSet imputData;
		imputData.userID		= userPropertiesPacket.userID;
		imputData.date			= userPropertiesPacket.date;
		imputData.strengh		= userPropertiesPacket.strengh;
		imputData.magicPower	= userPropertiesPacket.magicPower;
		imputData.agility		= userPropertiesPacket.agility;
		imputData.max_health	= userPropertiesPacket.maxHealth;
		imputData.max_mana		= userPropertiesPacket.maxMana;
		imputData.villager		= userPropertiesPacket.villager;
		imputData.level			= userPropertiesPacket.level;

		if (!SQLiteHelper->SetUserProperties(imputData)) {
			printf("setUserProperties query error.\n");
		}
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

