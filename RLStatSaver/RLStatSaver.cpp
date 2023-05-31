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

void RLStatSaver::gameEnd(std::string eventName)
{
	LOG("GAME END CODE EXECUTED");

	// If not in an online game, return.
	if (!gameWrapper->IsInOnlineGame()) { return; }

	LOG("IS IN ONLINE GAME");

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { return; }

	LOG("SERVER IS NOT NULL");

	GameSettingPlaylistWrapper playlist = server.GetPlaylist();
	if (!playlist) { return; }

	LOG("PLAYLIST IS NOT NULL");

	int playlistID = playlist.GetPlaylistId();

	LOG(std::to_string(playlistID));

	ArrayWrapper<PriWrapper> pris = gameWrapper->GetOnlineGame().GetPRIs();

	int localPlayerPRI;
	int localTeam = 0;
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

	// Max players is 8, so just set the array to that becaause idc
	Player players[8];

	// Get the local player instance and local team
	for (int i = 0; i < pris.Count(); i++) {
		bool isLocalPlayer = pris.Get(i).IsLocalPlayerPRI();

		if (isLocalPlayer) {
			localTeam = pris.Get(i).GetTeamNum();
			localPlayerPRI = i;
		}
	}

	// This one will only create the teammates.
	for (int i = 0; i < pris.Count(); i++) {
		playerTeam = pris.Get(i).GetTeamNum();
		if (playerTeam == localTeam) {
			playerName = pris.Get(i).GetPlayerName().ToString();
			goals = pris.Get(i).GetMatchGoals();
			assists = pris.Get(i).GetMatchAssists();
			saves = pris.Get(i).GetMatchSaves();
			shots = pris.Get(i).GetMatchShots();
			demos = pris.Get(i).GetMatchDemolishes();
			mvp = pris.Get(i).GetbMatchMVP();
			score = pris.Get(i).GetMatchScore();
			playerID = pris.Get(i).GetPlayerID();

			for (int j = 0; j < 8; j++) {
				// If array slot already has a player in it, skip this loop
				if (players[j].playerID > 1) {
					continue;
				}
				// Otherwise, create a new player object and put it in the array
				Player thisPlayer = Player(playerTeam, playerName, goals, assists, saves, shots, demos, mvp, score, playerID, mmr);
				players[j] = thisPlayer;
			}
		}
	}

	// Now this will create the opponents.
	// Yes it repeats code, but once again I don't care
	for (int i = 0; i < pris.Count(); i++) {
		playerTeam = pris.Get(i).GetTeamNum();
		if (playerTeam != localTeam && playerTeam < 2) {
			playerName = pris.Get(i).GetPlayerName().ToString();
			goals = pris.Get(i).GetMatchGoals();
			assists = pris.Get(i).GetMatchAssists();
			saves = pris.Get(i).GetMatchSaves();
			shots = pris.Get(i).GetMatchShots();
			demos = pris.Get(i).GetMatchDemolishes();
			mvp = pris.Get(i).GetbMatchMVP();
			score = pris.Get(i).GetMatchScore();
			playerID = pris.Get(i).GetPlayerID();

			for (int j = 0; j < 8; j++) {
				// If array slot already has a player in it, skip this loop
				if (players[j].playerID > 1) {
					continue;
				}
				// Otherwise, create a new player object and put it in the array
				Player thisPlayer = Player(playerTeam, playerName, goals, assists, saves, shots, demos, mvp, score, playerID);
				players[j] = thisPlayer;
			}
		}
	}

	// Get team size
	int lobbySize = 0;
	for (int i = 0; i < 8; i++) {
		// If array slot already has a player in it, iterate 1 and 
		if (players[i].playerID > 1) {
			lobbySize++;
			continue;
		}
	}

	// This will prevent a crash or bad data in an odd numbered lobby
	if (lobbySize % 2 != 0) {
		return;
	}

	// Now add up the first half of the player's goals and the second half of the player's goals to get the score.
	int playerTeamGoals = 0;
	for (int i = 0; i < lobbySize / 2; i++) {
		playerTeamGoals += players[i].goals;
	}
	int opponentTeamGoals = 0;
	for (int i = lobbySize / 2; i < lobbySize; i++) {
		opponentTeamGoals += players[i].goals;
	}

	LOG(players[0].playerName);

	if (playlistID == 11) {

//			std::ofstream stream(gameWrapper->GetBakkesModPath() / "data" / "abc.txt");
	//	std::ofstream stream(gameWrapper->GetDataFolder() / "abc.txt"); //Note the removal of "data"
		//stream << "def";
	}
}