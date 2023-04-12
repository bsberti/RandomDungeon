service Leaderboard {

	oneway void setHighScore(1:i32 playerId, 2:i32 highScore);
	map<i32, i32> getTop20();
}