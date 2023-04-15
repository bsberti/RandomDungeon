struct LoginResult {
	1: i32 result,
	2: i32 playerId
}

struct UserProperties {
  1: i32 userID,
  2: i64 date,
  3: i32 strengh,
  4: i32 magicPower,
  5: i32 agility,
  6: i32 maxHealth,
  7: i32 maxMana,
  8: string villager,
  9: i32 level
}

service Leaderboard {
	oneway void setHighScore(1:i32 playerId, 2:i32 highScore);
	map<i32, i32> getTop20();
	LoginResult login(1:string email, 2:string password);
    bool createAccount(1:string email, 2:string hashedPassword, 3:string salt);
	UserProperties getUserProperties(1:i32 playerId)
	oneway void setUserProperties(1:UserProperties userPropertiesPacket);
}