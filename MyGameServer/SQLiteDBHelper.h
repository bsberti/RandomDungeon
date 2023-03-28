#pragma once

#include <sqlite/sqlite3.h>

class SQLiteDBHelper {
public:
	SQLiteDBHelper();
	~SQLiteDBHelper();

	void ConnectToDB(const char* dbName);

	void ExecuteQuery(const char* sql);
private:
	sqlite3* m_DB;
	bool m_IsConnected;
};