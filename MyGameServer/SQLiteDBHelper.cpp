#include "SQLiteDBHelper.h"

#include <iostream>

int callback(void*, int, char**, char**) {

}

SQLiteDBHelper::SQLiteDBHelper()
	: m_DB(nullptr)
	, m_IsConnected(false) 
{
	
}

SQLiteDBHelper::~SQLiteDBHelper() {
	if (m_DB) {
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
		printf("SQLiteDBHelper::ExecuteQuery not connected to DB\n");
		return;
	}

	char* errorMsg;
	void* arg;
	sqlite3_exec(m_DB, sql, callback, arg, &errorMsg);
}
