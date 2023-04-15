#pragma once
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/Leaderboard.h"

#include <SHA256.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class NetworkManager {
public:
    NetworkManager() : socket_(new TSocket("localhost", 9090)), transport_(new TBufferedTransport(socket_)), protocol_(new TBinaryProtocol(transport_)), client_(protocol_) {}
    void Start() { transport_->open(); }
    void setHighScore(const int32_t playerId, const int32_t highScore) { client_.setHighScore(playerId, highScore); }
    void getTop20(std::map<int32_t, int32_t>& top20) { client_.getTop20(top20); }

    bool login(std::string email, std::string password, bool& newLogin, int& playerID) {
        LoginResult result;

        client_.login(result, email, password);

        if (result.result != 0) {
            playerID = result.playerId;
            if (result.result == 1) {
                newLogin = true;
            }
            else if (result.result == 2) {
                newLogin = false;
            }
            return true;
        }
        else {
            return false;
        }
    }

    bool createAccount(std::string email, std::string password) {

        //Defining random salt
        std::string salt;
        salt = std::to_string(rand() * 100000);

        // Hashing salt + password
        std::string hashedPassword;
        SHA256 sha;
        sha.update(salt + password);
        uint8_t* digest = sha.digest();
        hashedPassword = SHA256::toString(digest);

        return client_.createAccount(email, hashedPassword, salt);
    }

    void getUserProperties(int32_t userID, UserProperties& userProperties) {
        client_.getUserProperties(userProperties, userID);
    };

    void setUserProperties(const UserProperties& imputData) {
        client_.setUserProperties(imputData);
    }

    ~NetworkManager() { transport_->close(); }
private:
    boost::shared_ptr<TSocket> socket_;
    boost::shared_ptr<TTransport> transport_;
    boost::shared_ptr<TProtocol> protocol_;
    LeaderboardClient client_;
};