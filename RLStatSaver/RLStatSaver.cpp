#include "pch.h"
#include "RLStatSaver.h"
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

	CarWrapper localCar = gameWrapper->GetLocalCar();
	if (!localCar) { return; }

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
	float mmr = 0;
	bool teammate;

	// Get the local player instance and team.
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
			playercount++;
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
		}
	}

	// Now this will create the opponents.
	for (int i = 0; i < pris.Count(); i++) {
		playerTeam = pris.Get(i).GetTeamNum();
		if (playerTeam != localTeam && playerTeam < 2) {
			playercount++;
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
		}
	}

	if (playlistID == 11) {

//			std::ofstream stream(gameWrapper->GetBakkesModPath() / "data" / "abc.txt");
	//	std::ofstream stream(gameWrapper->GetDataFolder() / "abc.txt"); //Note the removal of "data"
		//stream << "def";
	}
}