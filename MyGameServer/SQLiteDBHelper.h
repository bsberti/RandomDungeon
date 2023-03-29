#pragma once

#include "sqlite3.h"
#include <vector>
#include <string>

struct LeaderboardResultSet {
	std::string userId;
	std::string highScore;
};

class SQLiteDBHelper {
public:
	SQLiteDBHelper();
	~SQLiteDBHelper();

	LeaderboardResultSet* resultSet;

	void ConnectToDB(const char* dbName);

	void ExecuteQuery(const char* sql);
	//void IncludeLeaderboardQuery(const char* sql);
	//void UpdateLeaderboardQuery(const char* sql);

	std::vector<LeaderboardResultSet>* vec_SQLiteResult;
	int resultCount;

private:
	sqlite3* m_DB;
	bool m_IsConnected;

};