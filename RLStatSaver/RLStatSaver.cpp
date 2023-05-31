#include "pch.h"
#include "RLStatSaver.h"
#include "Player.h"
#include "fstream"
#include <iomanip>


BAKKESMOD_PLUGIN(RLStatSaver, "Stat Saver", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void RLStatSaver::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Hello im Statsaver plugin");

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&RLStatSaver::gameEnd, this, std::placeholders::_1));


	notifierToken = gameWrapper->GetMMRWrapper().RegisterMMRNotifier(
		[this](UniqueIDWrapper id) {
			float mmr = gameWrapper->GetMMRWrapper().GetPlayerMMR(id, 11);
			LOG("{} MMR is: {}", id.GetIdString(), mmr);
		}
	);

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
	LOG("Goodbye i was statsaver plugin");
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

		return playlistName;
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

	int playlistID = playlist.GetPlaylistId();

	ArrayWrapper<PriWrapper> pris = gameWrapper->GetOnlineGame().GetPRIs();

	int lobbySize = pris.Count();
	int localPlayerPRI;
	int localTeam;
	int localPlayerID;
	int playerTeam = 0;
	int playercount = 0;
	std::string playerName = "";
	int goals = 0;
	int assists = 0;
	int saves = 0;
	int shots = 0;
	int demos = 0;
	int mvp = 0;
	int score = 0;
	int playerID = 0;
	float mmr = 0;

	// Max players is 8, so just set the array to that becaause idc
	Player players[8];

	// Get the local player instance and local team
	for (int i = 0; i < pris.Count(); i++) {
		bool isLocalPlayer = pris.Get(i).IsLocalPlayerPRI();

		if (isLocalPlayer) {
			localTeam = pris.Get(i).GetTeamNum();
			localPlayerPRI = i;
			localPlayerID = pris.Get(i).GetPlayerID();
		}
	}


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
			mvp = pris.Get(i).GetbMatchMVP();
			score = pris.Get(i).GetMatchScore();
			playerID = pris.Get(i).GetPlayerID();
			mmr = gameWrapper->GetMMRWrapper().GetPlayerMMR(playerID, playlistID);

			// Create a new player object and put it in the array
			Player thisPlayer = Player(playerTeam, playerName, goals, assists, saves, shots, demos, mvp, score, playerID);
			players[i] = thisPlayer;
		}
	}

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


	// Get the playlist in an easy to read format
	std::string playlistName = playlistIDtoName(playlistID);
	std::string fileName = playlistName + ".csv";

	// Output the results in a .csv file
	std::ofstream stream(gameWrapper->GetDataFolder() / fileName, std::ios_base::app);

	// Fill the top row with the proper labels
	stream << "TEAM COLOR, " << "NAME, " << "GOALS, " << "ASSISTS, " << "SAVES, " << "SHOTS, " << "DEMOS, " << "MVP, " << "SCORE, " << "PLAYERID, " << "TEAM GOALS, " << "W/L\n";

	// Iterate through each teammate and output the results
	for (int i = 0; i < lobbySize; i++) {
		if (players[i].playerTeam == localTeam) {
			stream << localTeamColor << ", " << players[i].playerName << ", " << players[i].goals << ", "
				<< players[i].assists << ", " << players[i].saves << ", " << players[i].shots << ", "
				<< players[i].demos << ", " << players[i].mvp << ", " << players[i].score << ", "
				<< playerTeamGoals << ", " << playerWorL << "\n";
		}
	}

	// Iterate through each opponent and output the results
	for (int i = 0; i < lobbySize; i++) {
		if (players[i].playerTeam != localTeam) {
			stream << opponentTeamColor << ", " << players[i].playerName << ", " << players[i].goals << ", "
				<< players[i].assists << ", " << players[i].saves << ", " << players[i].shots << ", "
				<< players[i].demos << ", " << players[i].mvp << ", " << players[i].score << ", "
				<< opponentTeamGoals << ", " << opponentWorL << "\n";
		}
	}

	// Finish by putting a 2 line gap between this and the next game
	stream << "\n\n";
}



