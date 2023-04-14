#include "SQLiteDBHelper.h"

#include <iostream>
#include <ctime>
#include <SHA256.h>

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

int ResultCallbackWebAuth(void* list, int columnsCount, char** data, char** columnsName) {
	int idx;

	std::vector<WebAuthResultSet>& queryResultList = *(std::vector<WebAuthResultSet>*)list;
	WebAuthResultSet queryResult;

	printf("There are %d column(s)\n", columnsCount);

	for (int i = 0; i < columnsCount; i++) {
		printf("The data in column \"%s\" is: %s\n", columnsName[i], data[i]);
		if (strcmp(columnsName[i], "id") == 0) {
			queryResult.id = data[i];
		}
		else if (strcmp(columnsName[i], "email") == 0) {
			queryResult.email = data[i];
		}
		else if (strcmp(columnsName[i], "hashed_password") == 0) {
			queryResult.hashedPassword = data[i];
		}
		else if (strcmp(columnsName[i], "salt") == 0) {
			queryResult.salt = data[i];
		}
	}

	queryResultList.push_back(queryResult);

	printf("\n");

	return 0;
}

int ResultCallbackUsers(void* list, int columnsCount, char** data, char** columnsName) {
	int idx;

	std::vector<UsersResultSet>& queryResultList = *(std::vector<UsersResultSet>*)list;
	UsersResultSet queryResult;

	printf("There are %d column(s)\n", columnsCount);

	for (int i = 0; i < columnsCount; i++) {
		printf("The data in column \"%s\" is: %s\n", columnsName[i], data[i]);
		if (strcmp(columnsName[i], "id") == 0) {
			queryResult.id = data[i];
		}
		else if (strcmp(columnsName[i], "last_login") == 0) {
			queryResult.last_login = data[i];
		}
		else if (strcmp(columnsName[i], "creation_date") == 0) {
			queryResult.creation_date = data[i];
		}
	}

	queryResultList.push_back(queryResult);

	printf("\n");

	return 0;
}

int ResultCallbackUsersProperties(void* list, int columnsCount, char** data, char** columnsName) {
	int idx;

	std::vector<userPropertiesResultSet>& queryResultList = *(std::vector<userPropertiesResultSet>*)list;
	userPropertiesResultSet queryResult;

	printf("There are %d column(s)\n", columnsCount);

	for (int i = 0; i < columnsCount; i++) {
		printf("The data in column \"%s\" is: %s\n", columnsName[i], data[i]);
		if (strcmp(columnsName[i], "userID") == 0) {
			queryResult.userID = atoi(data[i]);
		}
		else if (strcmp(columnsName[i], "date") == 0) {
			queryResult.date = data[i];
		}
		else if (strcmp(columnsName[i], "strengh") == 0) {
			queryResult.strengh = atoi(data[i]);
		}
		else if (strcmp(columnsName[i], "magic_power") == 0) {
			queryResult.magicPower = atoi(data[i]);
		}
		else if (strcmp(columnsName[i], "agility") == 0) {
			queryResult.agility = atoi(data[i]);
		}
		else if (strcmp(columnsName[i], "max_health") == 0) {
			queryResult.max_health = atoi(data[i]);
		}
		else if (strcmp(columnsName[i], "max_mana") == 0) {
			queryResult.max_mana = atoi(data[i]);
		}
		else if (strcmp(columnsName[i], "villager") == 0) {
			queryResult.villager = data[i];
		}
		else if (strcmp(columnsName[i], "level") == 0) {
			queryResult.level = atoi(data[i]);
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

bool SQLiteDBHelper::UpdateUser(std::string userID, bool& newLogin) {
	std::string query =
		"UPDATE users SET last_login = DATETIME('now', 'localtime') WHERE id = '" + userID + "';";

	char* errorMsg;
	std::vector<UsersResultSet> resultList;

	int result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackUsers, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	query = "SELECT * FROM users WHERE id = '" + userID + "';";
	result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackUsers, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	if (resultList.size() != 0) {

		time_t now = time(0);
		tm currentTime;
		localtime_s(&currentTime, &now);

		Date currentDate(currentTime.tm_year + 1900, currentTime.tm_mon + 1, currentTime.tm_mday);
		std::string dbDateString = resultList[0].last_login;
		int year = std::stoi(dbDateString.substr(0, 4));
		int month = std::stoi(dbDateString.substr(5, 2));
		int day = std::stoi(dbDateString.substr(8, 2));

		Date dbDate(year, month, day);

		if (currentDate == dbDate)
			newLogin = false;
		else
			newLogin = true;

		return true;
	}
	else {
		return false;
	}
}

bool SQLiteDBHelper::CreateUser(std::string userID) {
	std::string query =
		"INSERT INTO users (id, last_login, creation_date) VALUES (" +
		userID + ", DATETIME('now', 'localtime'), DATE('now', 'localtime'));";

	char* errorMsg;
	std::vector<UsersResultSet> resultList;

	int result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackUsers, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	return true;
}

bool SQLiteDBHelper::Login(std::string email, std::string password, bool& newLogin) {
	if (!m_IsConnected) {
		printf("SQLiteDBHelper::Login: not connected to a DB\n");
		return false;
	}

	bool isNewLogin = false;
	char* errorMsg;
	std::vector<WebAuthResultSet> resultList;
	std::string query = "SELECT * FROM web_auth WHERE email = '" + email + "';";

	int result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackWebAuth, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	if (resultList.size() == 1) {

		std::string currentSal = resultList[0].salt;

		// Hashing salt + password
		std::string hashedPassword;
		SHA256 sha;
		// The packets password is not hashed
		// because it's comming from a login request
		sha.update(currentSal + password);
		uint8_t* digest = sha.digest();
		hashedPassword = SHA256::toString(digest);

		if (hashedPassword == resultList[0].hashedPassword) {
			// Login successfull
			if (UpdateUser(resultList[0].id, isNewLogin)) {
				newLogin = isNewLogin;
				return true;
			}
			else {
				printf("Invalid Credentials!\n");
				return false;
			}
		}
		else {
			printf("Invalid Credentials!\n");
			return false;
		}
	}
	else {
		printf("Invalid Credentials!\n");
		return false;
	}
}

bool SQLiteDBHelper::CreateAccount(std::string email, std::string hashedPassword, std::string salt) {
	if (!m_IsConnected) {
		printf("SQLiteDBHelper::CreateAccount: not connected to a DB\n");
		return false;
	}

	bool isNewLogin = false;
	char* errorMsg;
	std::vector<WebAuthResultSet> resultList;
	std::string query = "SELECT * FROM web_auth WHERE email = '" + email + "';";

	int result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackWebAuth, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	if (resultList.size() == 0) {
		query = "INSERT INTO web_auth (email, hashed_password, salt) VALUES ('" +
			email + "', '" + hashedPassword + "', '" + salt + "');";

		result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackWebAuth, &resultList, &errorMsg);
		if (result != SQLITE_OK) {
			printf("Failed to execute our query with erro code: %d!\n", result);
			return false;
		}

		query = "SELECT * FROM web_auth WHERE email = '" + email + "';";
		int result = sqlite3_exec(m_DB, query.c_str(), ResultCallbackWebAuth, &resultList, &errorMsg);
		if (result != SQLITE_OK) {
			printf("Failed to execute our query with erro code: %d!\n", result);
			return false;
		}

		if (resultList.size() == 1) {
			std::string newUserId = resultList[0].id;
			if (CreateUser(newUserId)) {
				return true;
			}
			else {
				printf("Invalid Credentials!\n");
				return false;
			}
		}
		else {
			printf("Failed to execute our query with erro code: %d!\n", result);
			return false;
		}
	}
	else {
		printf("User already created.\n");
		return false;
	}
	
}

bool SQLiteDBHelper::GetUserProperties(userPropertiesResultSet& recordSet, std::string userID) {
	if (!m_IsConnected) {
		printf("SQLiteDBHelper::GetUserProperties: not connected to a DB\n");
		return false;
	}

	std::string sql = "SELECT * FROM user_properties WHERE userID = " + userID + ";";

	char* errorMsg;
	std::vector<userPropertiesResultSet> resultList;
	int result = sqlite3_exec(m_DB, sql.c_str(), ResultCallbackUsersProperties, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	if (resultList.size() == 1) {
		recordSet = resultList[0];
		return true;
	}
	else {
		return true;
	}
}

bool SQLiteDBHelper::SetUserProperties(userPropertiesResultSet data) {
	if (!m_IsConnected) {
		printf("SQLiteDBHelper::SetUserProperties: not connected to a DB\n");
		return false;
	}

	std::string sql = "SELECT * FROM user_properties WHERE userID = " + std::to_string(data.userID) + ";";

	char* errorMsg;
	std::vector<userPropertiesResultSet> resultList;
	int result = sqlite3_exec(m_DB, sql.c_str(), ResultCallbackUsersProperties, &resultList, &errorMsg);
	if (result != SQLITE_OK) {
		printf("Failed to execute our query with erro code: %d!\n", result);
		return false;
	}

	if (resultList.size() == 1) {
		sql = "UPDATE user_properties SET date = julianday('now', 'localtime'), strengh = " + std::to_string(data.strengh) + 
			", magic_power = " + std::to_string(data.magicPower) + 
			", agility = " + std::to_string(data.agility) + 
			", max_health = " + std::to_string(data.max_health) + 
			", max_mana = " + std::to_string(data.max_mana) + 
			", villager = '" + data.villager + 
			"', level = " + std::to_string(data.level) + 
			" WHERE userID = " + std::to_string(data.userID) + ";";

		result = sqlite3_exec(m_DB, sql.c_str(), ResultCallbackUsers, &resultList, &errorMsg);
		if (result != SQLITE_OK) {
			printf("Failed to execute our query with erro code: %d!\n", result);
			return false;
		}
	}
	else {
		printf("Failed to retrieve database information with userID: %d", data.userID);
		return false;
	}

	return true;
}