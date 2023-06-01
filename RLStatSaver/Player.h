#pragma once
#include <string>

class Player
{
public:
	// Default constructor
	Player() { playerID = -1;  newMMR = -1; return; };
	// Player constructor to save stats
	Player(int playerTeam, std::string playerName, int goals, int assists, int saves, int shots, int demos, int mvp, int score, int playerID, float oldMMR, float newMMR)
	{
		this->playerTeam = playerTeam;
		this->playerName = playerName;
		this->goals = goals;
		this->assists = assists;
		this->saves = saves;
		this->shots = shots;
		this->demos = demos;
		this->mvp = mvp;
		this->score = score;
		this->playerID = playerID;
		this->oldMMR = oldMMR;
		this->newMMR = newMMR;
	}
	int playerTeam;
	std::string playerName;
	int goals;
	int assists;
	int saves;
	int shots;
	int demos;
	int mvp;
	int score;
	int playerID;
	float oldMMR;
	float newMMR;
};