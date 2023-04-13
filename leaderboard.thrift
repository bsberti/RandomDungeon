service Leaderboard {

	oneway void setHighScore(1:i32 playerId, 2:i32 highScore);
	map<i32, i32> getTop20();
	bool login(1:string email, 2:string password, 3:bool newLogin);
    bool createAccount(1:string email, 2:string hashedPassword, 3:string salt);

}