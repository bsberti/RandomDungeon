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
    
    bool login(std::string email, std::string password, bool& newLogin) {
        bool isNewLogin = false;
        if (client_.login(email, password, isNewLogin)) {
            newLogin = isNewLogin;
            return true;
        }
        else
            return false;
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

    ~NetworkManager() { transport_->close(); }
private:
    boost::shared_ptr<TSocket> socket_;
    boost::shared_ptr<TTransport> transport_;
    boost::shared_ptr<TProtocol> protocol_;
    LeaderboardClient client_;
};

//class NetworkManager {
//public:
//    NetworkManager() {
//        socket_ = std::make_shared<apache::thrift::transport::TSocket>("localhost", 9090);
//        transport_ = std::make_shared<apache::thrift::transport::TBufferedTransport>(socket_);
//        protocol_ = std::make_shared<apache::thrift::protocol::TBinaryProtocolT<apache::thrift::transport::TBufferedTransport>>(transport_);
//
//        client_ = std::make_shared<LeaderboardClient>(protocol_);
//    }
//
//    void Start() {
//        transport_->open();
//    }
//
//    void setHighScore(int game_id, int score) {
//        client_->setHighScore(game_id, score);
//    }
//
//    void getTop20(std::map<int32_t, int32_t>& top20) {
//        client_->getTop20(top20);
//    }
//
//private:
//    std::shared_ptr<apache::thrift::transport::TSocket> socket_;
//    std::shared_ptr<apache::thrift::transport::TBufferedTransport> transport_;
//    std::shared_ptr<apache::thrift::protocol::TBinaryProtocolT<apache::thrift::transport::TBufferedTransport>> protocol_;
//    std::shared_ptr<LeaderboardClient> client_;
//};
