#pragma once

#include "sqlite3.h"
#include <vector>
#include <string>

struct LeaderboardResultSet {
	std::string userId;
	std::string highScore;
};

struct WebAuthResultSet {
	std::string id;
	std::string email;
	std::string hashedPassword;
	std::string salt;
};

struct UsersResultSet {
	std::string id;
	std::string last_login;
	std::string creation_date;
};

class Date {
public:
	Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

	bool operator==(const Date& other) const {
		return (year_ == other.year_) && (month_ == other.month_) && (day_ == other.day_);
	}

private:
	int year_;
	int month_;
	int day_;
};


class SQLiteDBHelper {
public:
	SQLiteDBHelper();
	~SQLiteDBHelper();

	LeaderboardResultSet* resultSet;

	void ConnectToDB(const char* dbName);

	void ExecuteQuery(const char* sql);
	bool Login(std::string email, std::string password, bool& newLogin);
	bool UpdateUser(std::string userID, bool& newLogin);
	bool CreateAccount(std::string email, 
		std::string hashedPassword, std::string salt);
	bool CreateUser(std::string userID);

	std::vector<LeaderboardResultSet>* vec_SQLiteResult;
	int resultCount;

private:
	sqlite3* m_DB;
	bool m_IsConnected;

};