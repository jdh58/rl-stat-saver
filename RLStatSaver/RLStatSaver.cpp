#include "pch.h"
#include "RLStatSaver.h"
#include "Player.h"
#include "fstream"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

BAKKESMOD_PLUGIN(RLStatSaver, "RLStatSaver", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

// Declaring these globally so i can access them anywhere
int playlistID = 0;
int lobbySize = 0;
int localTeam = 0;
int playerTeam = 0;
int playercount = 0;
std::string playerName = "";
int goals = 0;
int assists = 0;
int saves = 0;
int shots = 0;
int demos = 0;
int score = 0;
int playerID = 0;
std::string uniqueID;
float MMR = 0;

// Max players is 8, so just set the array to that because idc
Player players[8];

void RLStatSaver::onLoad()
{
	_globalCvarManager = cvarManager;
	gameWrapper->HookEvent("Function ProjectX.GRI_X.EventGameStarted", std::bind(&RLStatSaver::gameStart, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&RLStatSaver::gameEnd, this, std::placeholders::_1));
	

	// Hook into MMR tracking (i think?)
	notifierToken = gameWrapper->GetMMRWrapper().RegisterMMRNotifier(
		[this](UniqueIDWrapper id) {
			float mmr = gameWrapper->GetMMRWrapper().GetPlayerMMR(id, 11);
			LOG("{} MMR is: {}", id.GetIdString(), mmr);
		}
	);
	//  We need the params so we hook with caller, but there is no wrapper for the HUD
	gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ServerWrapper caller, void* params, std::string eventname) {
			onStatTickerMessage(params);
		});

	// hooked whenever the primary player earns a stat
	gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatEvent",
		[this](ServerWrapper caller, void* params, std::string eventname) {
			onStatEvent(params);
		});



	//cvarManager->log("Plugin loaded!");

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	cvarManager->log("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&RLStatSaver::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&RLStatSaver::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&RLStatSaver::YourPluginMethod, this);
}

void RLStatSaver::onUnload()
{
}

std::string playlistIDtoName(int playlistNumber) {
	std::string playlistName;

	switch (playlistNumber) {
	case -2:
		playlistName = "Intermission";
		break;
	case 0:
		playlistName = "Casual";
		break;
	case 1:
		playlistName = "Duel";
		break;
	case 2:
		playlistName = "Doubles";
		break;
	case 3:
		playlistName = "Standard";
		break;
	case 4:
		playlistName = "Chaos";
		break;
	case 6:
		playlistName = "Private_Match";
		break;
	case 7:
		playlistName = "Season";
		break;
	case 8:
		playlistName = "Exhibition";
		break;
	case 9:
		playlistName = "Training";
		break;
	case 10:
		playlistName = "Duel_Ranked";
		break;
	case 11:
		playlistName = "Doubles_Ranked";
		break;
	case 13:
		playlistName = "Standard_Ranked";
		break;
	case 15:
		playlistName = "Snow_Day";
		break;
	case 16:
		playlistName = "Rocket_Labs";
		break;
	case 17:
		playlistName = "Hoops";
		break;
	case 18:
		playlistName = "Rumble";
		break;
	case 19:
		playlistName = "Workshop";
		break;
	case 20:
		playlistName = "Custom_Training_Editor";
		break;
	case 21:
		playlistName = "Custom_Training";
		break;
	case 22:
		playlistName = "Tournament_Match_Custom";
		break;
	case 23:
		playlistName = "Dropshot";
		break;
	case 24:
		playlistName = "Local_Match";
		break;
	case 26:
		playlistName = "External_Match_Ranked";
		break;
	case 27:
		playlistName = "Hoops_Ranked";
		break;
	case 28:
		playlistName = "Rumble_Ranked";
		break;
	case 29:
		playlistName = "Dropshot_Ranked";
		break;
	case 30:
		playlistName = "Snow_Day_Ranked";
		break;
	case 31:
		playlistName = "Ghost_Hunt";
		break;
	case 32:
		playlistName = "Beach_Ball";
		break;
	case 33:
		playlistName = "Spike_Rush";
		break;
	case 34:
		playlistName = "Tournament_Match_Automatic";
		break;
	case 35:
		playlistName = "Rocket_Labs";
		break;
	case 37:
		playlistName = "Dropshot_Rumble";
		break;
	case 38:
		playlistName = "Heatseeker";
		break;
	case 41:
		playlistName = "Boomer_Ball";
		break;
	case 43:
		playlistName = "Heatseeker_Doubles";
		break;
	case 44:
		playlistName = "Winter_Breakaway";
		break;
	case 46:
		playlistName = "Gridiron";
		break;
	case 47:
		playlistName = "Super_Cube";
		break;
	case 48:
		playlistName = "Tactical_Rumble";
		break;
	case 49:
		playlistName = "Spring_Loaded";
		break;
	case 50:
		playlistName = "Speed_Demon";
		break;
	case 52:
		playlistName = "Gotham_City_Rumble";
		break;
	case 54:
		playlistName = "Knockout";
		break;
	case 55:
		playlistName = "confidential_thirdwheel_test";
		break;
	default:
		playlistName = "Invalid_Playlist";
		break;
	}

	return playlistName;
}

void RLStatSaver::gameStart(std::string eventName)
{
	// If not in an online game, return.
	if (!gameWrapper->IsInOnlineGame()) { return; }

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	GameSettingPlaylistWrapper playlist = server.GetPlaylist();
	if (!playlist) { return; }

	ArrayWrapper<PriWrapper> pris = gameWrapper->GetOnlineGame().GetPRIs();

	// Re-initialize here. I don't think I have to but just in case I guess.
	playlistID = playlist.GetPlaylistId();
	lobbySize = pris.Count();
	localTeam;
	playerTeam = 0;
	playercount = 0;
	playerName = "";
	goals = 0;
	assists = 0;
	saves = 0;
	shots = 0;
	demos = 0;
	score = 0;
	playerID = 0;
	MMR = 0;

	int totalScore = 0;

	for (int i = 0; i < pris.Count(); i++) {
		totalScore += pris.Get(i).GetMatchGoals();
	}

	// Get the local player instance and local team
	for (int i = 0; i < pris.Count(); i++) {
		bool isLocalPlayer = pris.Get(i).IsLocalPlayerPRI();

		if (isLocalPlayer) {
			localTeam = pris.Get(i).GetTeamNum();
		}
	}

	if (totalScore <= 0) {
		// This will create a player instance for each player
		for (int i = 0; i < pris.Count(); i++) {
			playerTeam = pris.Get(i).GetTeamNum();
			if (playerTeam < 2) {
				playerName = pris.Get(i).GetPlayerName().ToString();
				goals = pris.Get(i).GetMatchGoals();
				assists = pris.Get(i).GetMatchAssists();
				saves = pris.Get(i).GetMatchSaves();
				shots = pris.Get(i).GetMatchShots();
				demos = pris.Get(i).GetMatchDemolishes();
				score = pris.Get(i).GetMatchScore();
				playerID = pris.Get(i).GetPlayerID();
				uniqueID = pris.Get(i).GetUniqueIdWrapper().GetIdString();

				// Get MMR only if teammates
				if (pris.Get(i).GetTeamNum() == localTeam) {
					MMR = gameWrapper->GetMMRWrapper().GetPlayerMMR(
						pris.Get(i).GetUniqueIdWrapper(),
						gameWrapper->GetMMRWrapper().GetCurrentPlaylist());
				}
				else {
					MMR = 0;
				}

				// Create a new player object and put it in the array
				Player thisPlayer = Player(playerTeam, playerName, goals, assists, saves, shots, demos, score, playerID, uniqueID, MMR);
				players[i] = thisPlayer;
			}
		}
	}
	else {
		updateStats();
	}
}

void RLStatSaver::gameEnd(std::string eventName)
{
	// If not in an online game, return.
	if (!gameWrapper->IsInOnlineGame()) { return; }

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	GameSettingPlaylistWrapper playlist = server.GetPlaylist();
	if (!playlist) { return; }

	ArrayWrapper<PriWrapper> pris = gameWrapper->GetOnlineGame().GetPRIs();

	// This will be the final update for the player instances
	updateStats();

	// Now add up the teammate player's goals and the opponent player's goals to get the score.
	int playerTeamGoals = 0;
	int opponentTeamGoals = 0;
	for (int i = 0; i < lobbySize; i++) {
		if (players[i].playerTeam == localTeam) {
			playerTeamGoals += players[i].goals;
		}
		else {
			opponentTeamGoals += players[i].goals;
		}
	}

	std::string playerWorL;
	std::string opponentWorL;
	if (playerTeamGoals > opponentTeamGoals) {
		playerWorL = "WIN";
		opponentWorL = "LOSS";
	}
	else {
		playerWorL = "LOSS";
		opponentWorL = "WIN";
	}

	std::string localTeamColor;
	std::string opponentTeamColor;
	if (localTeam == 0) {
		localTeamColor = "Blue";
		opponentTeamColor = "Orange";
	} else {
		localTeamColor = "Orange";
		opponentTeamColor = "Blue";
	}

	// Get the current date and format it as a timestamp
	// Get current time
    std::time_t currentTime = std::time(nullptr);

    // Convert time to local time
    std::tm* localTime = std::localtime(&currentTime);

    // Format time as "MM/dd/yyyy hh:mm"
    std::ostringstream oss;
    oss << std::put_time(localTime, "%m/%d/%Y %H:%M");
    std::string timestamp = oss.str();

	// Extract the year
	std::ostringstream yearStream;
	yearStream << std::put_time(localTime, "%Y");
	std::string year = yearStream.str();


	// Get the playlist name in a string
	std::string playlistName = playlistIDtoName(playlistID);
	std::string fileName = playlistName + ".csv";

	// Create the RLStatSaver directory if it doesn't exist
	std::filesystem::path rlStatSaverDirPath = (gameWrapper->GetDataFolder() / "RLStatSaver");
	std::filesystem::create_directory(rlStatSaverDirPath);

	// Create the year directory if it doesn't exist
	std::filesystem::path yearDirPath = rlStatSaverDirPath / year;
	std::filesystem::create_directory(yearDirPath);

	// Output the results in a .csv file
	std::ofstream stream(gameWrapper->GetDataFolder() / "RLStatSaver" / year / fileName, std::ios_base::app);

	// Fill the top row with the proper labels
	stream << "TEAM COLOR, " << "NAME, " << "GOALS, " << "ASSISTS, " << "SAVES, " << "SHOTS, " << "DEMOS, " << "SCORE, " << "TEAM GOALS, " << "W/L, " << "TIMESTAMP, " << "PLAYERID\n";

	// Iterate through until you get to the local user, and output their stats first
	for (int i = 0; i < lobbySize; i++) {
		bool isLocalPlayer = pris.Get(i).IsLocalPlayerPRI();
		if (isLocalPlayer) {
			stream << localTeamColor << ", " << players[i].playerName << ", " << players[i].goals << ", "
				<< players[i].assists << ", " << players[i].saves << ", " << players[i].shots << ", "
				<< players[i].demos << ", " << players[i].score << ", "
				<< players[i].MMR << ", " << playerTeamGoals << ", " << playerWorL << ", " << timestamp << ", "
				<< players[i].uniqueID << "\n";
		}
	}

	// Iterate through each teammate and output the results
	for (int i = 0; i < lobbySize; i++) {
		bool isLocalPlayer = pris.Get(i).IsLocalPlayerPRI();
		if (!isLocalPlayer && players[i].playerTeam == localTeam) {
			stream << localTeamColor << ", " << players[i].playerName << ", " << players[i].goals << ", "
				<< players[i].assists << ", " << players[i].saves << ", " << players[i].shots << ", "
				<< players[i].demos << ", " << players[i].score << ", "
				<< players[i].MMR << ", " << playerTeamGoals << ", " << playerWorL << ", " << timestamp << ", "
				<< players[i].uniqueID << "\n";
		}
	}

	// Iterate through each opponent and output the results
	for (int i = 0; i < lobbySize; i++) {
		if (players[i].playerTeam != localTeam) {
			stream << opponentTeamColor << ", " << players[i].playerName << ", " << players[i].goals << ", "
				<< players[i].assists << ", " << players[i].saves << ", " << players[i].shots << ", "
				<< players[i].demos << ", " << players[i].score << ", "
				<< players[i].MMR << ", " << opponentTeamGoals << ", " << opponentWorL << ", " << timestamp << ", "
				<< players[i].uniqueID << "\n";
		}
	}

	// Finish by putting a 2 line gap between this and the next game
	stream << "\n\n";
}

// The structure of a ticker stat event
struct StatTickerParams {
	// person who got a stat
	uintptr_t Receiver;
	// person who is victim of a stat (only exists for demos afaik)
	uintptr_t Victim;
	// wrapper for the stat event
	uintptr_t StatEvent;
};

// structure of a stat event
struct StatEventParams {
	// always primary player
	uintptr_t PRI;
	// wrapper for the stat event
	uintptr_t StatEvent;
};

void RLStatSaver::onStatTickerMessage(void* params) {
	StatTickerParams* pStruct = (StatTickerParams*)params;
	PriWrapper receiver = PriWrapper(pStruct->Receiver);
	PriWrapper victim = PriWrapper(pStruct->Victim);
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);

	// Demos don't track, so we have to track them manually
	if (statEvent.GetEventName() == "Demolish") {
		for (int i = 0; i < lobbySize; i++) {
			if (players[i].playerID == receiver.GetPlayerID()) {
				players[i].demos = players[i].demos + 1;
			}
		}
	}

	// On each stat event, update all the stats for each player (to prevent them from leaving and not being tracked)
	updateStats();
}

void RLStatSaver::onStatEvent(void* params) {
	StatEventParams* pStruct = (StatEventParams*)params;
	PriWrapper playerPRI = PriWrapper(pStruct->PRI);
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);

	// On each stat event, update all the stats for each player (to prevent them from leaving and not being tracked)
	updateStats();
}


void RLStatSaver::updateStats()
{
	// If not in an online game, return.
	if (!gameWrapper->IsInOnlineGame()) { return; }

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	GameSettingPlaylistWrapper playlist = server.GetPlaylist();
	if (!playlist) { return; }

	ArrayWrapper<PriWrapper> pris = gameWrapper->GetOnlineGame().GetPRIs();

	// This will update the player instance for each player
	for (int i = 0; i < pris.Count(); i++) {
		playerTeam = pris.Get(i).GetTeamNum();
		if (playerTeam < 2) {
			playerName = pris.Get(i).GetPlayerName().ToString();
			goals = pris.Get(i).GetMatchGoals();
			assists = pris.Get(i).GetMatchAssists();
			saves = pris.Get(i).GetMatchSaves();
			shots = pris.Get(i).GetMatchShots();
			score = pris.Get(i).GetMatchScore();
			playerID = pris.Get(i).GetPlayerID();
			uniqueID = pris.Get(i).GetUniqueIdWrapper().GetIdString();

			// Check if this is the same player we are updating.
			// This is to prevent a case where, for example, Player 3 leaves the game, and we overwrite Player 3's stats with Player 4's
			// By iterating through the max lobby size we we will always find the correct match.
			for (int j = 0; j < lobbySize; j++) {
				if (playerID == players[j].playerID) {
					int thisDemos = players[j].demos;
					float thisMMR = players[j].MMR;
					// Create a new player object and put it in the array
					Player thisPlayer = Player(playerTeam, playerName, goals, assists, saves, shots, thisDemos, score, playerID, uniqueID, thisMMR);
					players[j] = thisPlayer;
					break;
				}
			}
		}
	}
	return;
}