#include "SQLiteDBHelper.h"

#include <iostream>

int ResultCallback(void* list, int columnsCount, char** data, char** columnsName) {
	int idx;

	std::vector<LeaderboardResultSet>& queryResultList = *(std::vector<LeaderboardResultSet>*)list;
	LeaderboardResultSet queryResult;

	printf("There are %d column(s)\n", columnsCount);

	for (int i = 0; i < columnsCount; i++) {
		printf("The data in column \"%s\" is: %s\n", columnsName[i], data[i]);
		if (strcmp(columnsName[i], "userID") == 0) {
			queryResult.userId = data[i];
		}
		else if (strcmp(columnsName[i], "highScore") == 0) {
			queryResult.highScore = data[i];
		}
	}

	queryResultList.push_back(queryResult);

	printf("\n");

	return 0;
}

SQLiteDBHelper::SQLiteDBHelper()
	: m_DB(nullptr)
	, m_IsConnected(false) {

	resultSet = new LeaderboardResultSet();
	vec_SQLiteResult = new std::vector< LeaderboardResultSet>();
	resultCount = 0;
}

SQLiteDBHelper::~SQLiteDBHelper() {
	if (m_DB) {
		printf("SQLiteDBHelper::~SQLiteDBHelper closed connection to our DB\n");
		sqlite3_close(m_DB);
	}
}

void SQLiteDBHelper::ConnectToDB(const char* dbName) {
	if (m_IsConnected) {
		printf("SQLiteDBHelper::ConnectToDB already connected to a DB\n");
		return;
	}

	int result = sqlite3_open(dbName, &m_DB);
	if (result != SQLITE_OK) {
		printf("Failed to open our SQLite DB '%d' with error code: %d\n", 
			dbName, result);
	}

	m_IsConnected = true;
}

void SQLiteDBHelper::ExecuteQuery(const char* sql) {
	if (!m_IsConnected) {
		printf("SQLiteDBHelper::ExecuteQuery: not connected to a DB\n");
		return;
	}

	char* errorMsg;
	std::vector<LeaderboardResultSet> resultList; 
	int result = sqlite3_exec(m_DB, sql, ResultCallback, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return;
	}

	vec_SQLiteResult = new std::vector<LeaderboardResultSet>(resultList);
}

//void SQLiteDBHelper::IncludeLeaderboardQuery(const char* sql) {
//
//}

//void SQLiteDBHelper::UpdateLeaderboardQuery(const char* sql) {
//
//}