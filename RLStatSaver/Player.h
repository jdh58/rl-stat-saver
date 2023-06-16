#pragma once
#include <string>

class Player
{
public:
	// Default constructor
	Player() { playerID = -1;  MMR = -1; return; };
	// Player constructor to save stats
	Player(int playerTeam, std::string playerTeamColor, std::string playerName, int goals, int assists, int saves, int shots, int demos, int score, int playerID, std::string uniqueID, float MMR)
	{
		this->playerTeam = playerTeam;
		this->playerTeamColor = playerTeamColor;
		this->playerName = playerName;
		this->goals = goals;
		this->assists = assists;
		this->saves = saves;
		this->shots = shots;
		this->demos = demos;
		this->score = score;
		this->playerID = playerID;
		this->uniqueID = uniqueID;
		this->MMR = MMR;
	}
	int playerTeam;
	std::string playerTeamColor;
	std::string playerName;
	int goals;
	int assists;
	int saves;
	int shots;
	int demos;
	int score;
	int playerID;
	std::string uniqueID;
	float MMR;
};