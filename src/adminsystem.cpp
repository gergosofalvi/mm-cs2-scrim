/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "protobuf/generated/usermessages.pb.h"

#include "adminsystem.h"
#include "KeyValues.h"
#include "interfaces/interfaces.h"
#include "icvar.h"
#include "playermanager.h"
#include "commands.h"
#include <ctime>
#include <stdio.h>
#include <string>
#include "filesystem.h"

extern IVEngineServer2 *g_pEngineServer2;
extern CEntitySystem *g_pEntitySystem;

CAdminSystem* g_pAdminSystem;

CUtlMap<uint32, FnChatCommandCallback_t> g_CommandList(0, 0, DefLessFunc(uint32));

CON_COMMAND_F(c_reload_admins, "Reload admin config", FCVAR_SPONLY | FCVAR_LINKED_CONCOMMAND)
{
	g_pAdminSystem->LoadAdmins();

	for (int i = 0; i < MAXPLAYERS; i++)
	{
		ZEPlayer* pPlayer = g_playerManager->GetPlayer(i);

		if (!pPlayer || pPlayer->IsFakeClient())
			continue;

		pPlayer->CheckAdmin();
	}
}
/*
CON_COMMAND_F(c_reload_infractions, "Reload admin config", FCVAR_SPONLY | FCVAR_LINKED_CONCOMMAND)
{
	g_pAdminSystem->LoadInfractions();

	for (int i = 0; i < MAXPLAYERS; i++)
	{
		ZEPlayer* pPlayer = g_playerManager->GetPlayer(i);

		if (!pPlayer || pPlayer->IsFakeClient())
			continue;

		pPlayer->CheckInfractions();
	}

	Message("Infractions reloaded\n");
}*/

CON_COMMAND_CHAT(scrim, "Scrim mode")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	

	char buf[256];
	//V_snprintf(buf, sizeof(buf), "exec %s", args[1]);

	g_pEngineServer2->ServerCommand("exec scrim");

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Scrim mode loaded, mr24");
	ClientPrintAll(HUD_PRINTTALK, CHAT_PREFIX"Available commands: \4.pause\1, \4.unpause\1");
	//ClientPrintAll(HUD_PRINTTALK, CHAT_PREFIX"Admin commands: \4.pracc\1, \4.scrim\1, \4.record\1, \4.stoprecord\1, \4.map\1");
}

CON_COMMAND_CHAT(pracc, "Practice mode")
{
	if (!player)
		return;
	
	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	char buf[256];
	//V_snprintf(buf, sizeof(buf), "exec %s", args[1]);

	g_pEngineServer2->ServerCommand("exec pracc");

	ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Practice mode loaded");
	ClientPrintAll(HUD_PRINTTALK, CHAT_PREFIX"Available commands: \4.pause\1, \4.unpause\1");
	//ClientPrintAll(HUD_PRINTTALK, CHAT_PREFIX"Admin commands: \4.pracc\1, \4.scrim\1, \4.record\1, \4.stoprecord\1, \4.map\1");
}

CON_COMMAND_CHAT(forceunpause, "Force unpause")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	ClientPrintAll(HUD_PRINTTALK, CHAT_PREFIX"Forced unpause");

	char buf[256];
	//V_snprintf(buf, sizeof(buf), "exec %s", args[1]);

	g_pEngineServer2->ServerCommand("mp_unpause_match");
}

CON_COMMAND_CHAT(restore, "Restore round")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	if (args.ArgC() < 2)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Usage: !restore <round number>");
		return;
	}

	char buf[256];
	
	//Save the round number
    char nRound[2];

    //Needed to build a string for the default restore, format "backup_round<round>.txt" the <round> with "XX" format like "backup_round02.txt"
    char backupFile[256] = "backup_round";
	char aux[256] = "";
        
	int nRounds = atoi(args[1]);
	
	V_snprintf(aux, MAX_PATH, "%s", backupFile);

	if ( nRounds < 10) V_snprintf(buf, MAX_PATH, "%s%i",aux, 0);

	V_snprintf(aux, MAX_PATH, "%s%i.txt", buf, nRounds);

	V_snprintf(buf, MAX_PATH, "mp_backup_restore_load_file %s", aux);

	ClientPrintAll(HUD_PRINTTALK, CHAT_PREFIX"\4Restored round \3%i", nRounds);

	g_pEngineServer2->ServerCommand(buf);
}

char demoName[128];
CON_COMMAND_CHAT(record, "Record demo")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	char buf[256];
	char actualTime[32];
	char actualMap[32];

	std::tm tm{};
	std::time_t result = std::time(nullptr);
	std::localtime(&result);
	std::strftime(actualTime, sizeof(actualTime), "%d%B_%H-%M", std::localtime(&result));

	V_snprintf(actualMap,MAX_PATH, "unknownMap");
	V_snprintf(demoName, MAX_PATH, "%s_%s", actualTime, actualMap);

	V_snprintf(buf, MAX_PATH, "tv_record gotv/%s", demoName);
	ClientPrint(player, HUD_PRINTTALK, buf);
	g_pEngineServer2->ServerCommand(buf);
	//V_snprintf(buf, MAX_PATH, "record gotv/%s", demoName);
/*
	FormatTime(actualTime, sizeof(actualTime), "%d%B_%H-%M", GetTimestamp());	//https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
	GetCurrentMap(actualMap, sizeof(actualMap));

	StrCat(demoName, 256, actualTime);
	StrCat(demoName, 256, "_");
	StrCat(demoName, 256, actualMap[3]);

	PrintToChat(client, "Demo will be saved at gotv/%s.dem", demoName);

	ServerCommand("tv_record gotv/%s", demoName);*/

}

CON_COMMAND_CHAT(stoprecord, "Stop demo recording")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	char buf[256];
	char command[256];
	
	V_snprintf(buf, MAX_PATH, "Demo saved at gotv/%s", demoName);
	ClientPrint(player, HUD_PRINTTALK, buf);

	V_snprintf(command, MAX_PATH, "tv_stoprecord");
	g_pEngineServer2->ServerCommand(command);
	
}

CON_COMMAND_CHAT(ban, "ban a player")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(iCommandPlayer);

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_BAN))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You don't have access to this command.");
		return;
	}

	if (args.ArgC() < 3)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Usage: !ban <name> <duration/0 (permanent)>");
		return;
	}

	int iNumClients = 0;
	int pSlot[MAXPLAYERS];

	if (g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlot) != ETargetType::PLAYER || iNumClients > 1)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Target too ambiguous.");
		return;
	}

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Target not found.");
		return;
	}

	char* end;
	int iDuration = strtol(args[2], &end, 10);

	if (*end)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Invalid duration.");
		return;
	}

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController* pTarget = (CBasePlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer* pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);

		if (pTargetPlayer->IsFakeClient())
			continue;

		CInfractionBase *infraction = new CBanInfraction(iDuration ? std::time(0) + iDuration : 0, pTargetPlayer->GetSteamId64());

		g_pAdminSystem->AddInfraction(infraction);
		infraction->ApplyInfraction(pTargetPlayer);
		g_pAdminSystem->SaveInfractions();

		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You have banned %s for %i minutes.", pTarget->GetPlayerName(), iDuration);
	}
}

CON_COMMAND_CHAT(mute, "mutes a player")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer* pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_BAN))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You don't have access to this command.");
		return;
	}

	if (args.ArgC() < 3)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Usage: !mute <name> <duration/0 (permanent)>");
		return;
	}

	int iNumClients = 0;
	int pSlot[MAXPLAYERS];

	g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlot);

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Target not found.");
		return;
	}

	char* end;
	int iDuration = strtol(args[2], &end, 10);

	if (*end)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Invalid duration.");
		return;
	}

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController* pTarget = (CBasePlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer* pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);

		if (pTargetPlayer->IsFakeClient())
			continue;

		CInfractionBase* infraction = new CMuteInfraction(iDuration ? std::time(0) + iDuration : 0, pTargetPlayer->GetSteamId64());

		g_pAdminSystem->AddInfraction(infraction);
		infraction->ApplyInfraction(pTargetPlayer);
		g_pAdminSystem->SaveInfractions();

		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You have muted %s for %i mins.", pTarget->GetPlayerName(), iDuration);
	}
}

CON_COMMAND_CHAT(kick, "kick a player")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer* pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_KICK))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You don't have access to this command.");
		return;
	}

	if (args.ArgC() < 2)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Usage: !kick <name>");
		return;
	}

	int iNumClients = 0;
	int pSlot[MAXPLAYERS];

	g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlot);

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Target not found.");
		return;
	}

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController* pTarget = (CBasePlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlot[i] + 1));

		if (!pTarget)
			continue;

		ZEPlayer* pTargetPlayer = g_playerManager->GetPlayer(pSlot[i]);
		
		g_pEngineServer2->DisconnectClient(pTargetPlayer->GetPlayerSlot().Get(), NETWORK_DISCONNECT_KICKED);

		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You have kicked %s.", pTarget->GetPlayerName());
	}
}

CON_COMMAND_CHAT(slay, "slay a player")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(player->GetPlayerSlot());

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_SLAY))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "You don't have access to this command.");
		return;
	}

	if (args.ArgC() < 2)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Usage: !bring <name>");
		return;
	}

	int iNumClients = 0;
	int pSlots[MAXPLAYERS];

	g_playerManager->TargetPlayerString(iCommandPlayer, args[1], iNumClients, pSlots);

	if (!iNumClients)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Target not found.");
		return;
	}

	for (int i = 0; i < iNumClients; i++)
	{
		CBasePlayerController *pTarget = (CBasePlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(pSlots[i] + 1));

		if (!pTarget)
			continue;

		// CBasePlayerPawn::CommitSuicide(bool bExplode, bool bForce)
		CALL_VIRTUAL(void, 354, pTarget->GetPawn(), false, true);

		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX "Brought %s.", pTarget->GetPlayerName());
	}
}

CON_COMMAND_CHAT(map, "change map")
{
	if (!player)
		return;

	int iCommandPlayer = player->GetPlayerSlot();

	ZEPlayer *pPlayer = g_playerManager->GetPlayer(iCommandPlayer);

	if (!pPlayer->IsAdminFlagSet(ADMFLAG_CHANGEMAP))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"You don't have access to this command.");
		return;
	}

	if (args.ArgC() < 2)
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Usage: !map <mapname>");
		return;
	}
/*
	if (!g_pEngineServer2->IsMapValid(args[1]))
	{
		ClientPrint(player, HUD_PRINTTALK, CHAT_PREFIX"Invalid map specified.");
		return;
	}
*/
	char buf[MAX_PATH];
	V_snprintf(buf, sizeof(buf), "changelevel de_%s", args[1]);
	g_pEngineServer2->ServerCommand(buf);
}

void CAdminSystem::LoadAdmins()
{
	m_vecAdmins.Purge();
	KeyValues* pKV = new KeyValues("admins");
	KeyValues::AutoDelete autoDelete(pKV);

	if (!pKV->LoadFromFile(g_pFullFileSystem, "addons/cs2scrim/configs/admins.cfg"))
	{
		Warning("Failed to load addons/cs2scrim/configs/admins.cfg\n");
		return;
	}
	for (KeyValues* pKey = pKV->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey())
	{
		const char *pszName = pKey->GetName();
		const char *pszSteamID = pKey->GetString("steam", nullptr);
		const char *pszFlags = pKey->GetString("flags", nullptr);

		if (!pszSteamID)
		{
			Warning("Admin entry %s is missing 'steam' key\n", pszName);
			return;
		}

		if (!pszFlags)
		{
			Warning("Admin entry %s is missing 'flags' key\n", pszName);
			return;
		}

		ConMsg("Loaded admin %s\n", pszName);
		ConMsg(" - Steam ID %5s\n", pszSteamID);
		ConMsg(" - Flags %5s\n", pszFlags);

		uint64 iFlags = ParseFlags(pszFlags);

		// Let's just use steamID64 for now
		m_vecAdmins.AddToTail(CAdmin(pszName, atoll(pszSteamID), iFlags));
	}
}

void CAdminSystem::LoadInfractions()
{
	m_vecInfractions.PurgeAndDeleteElements();
	KeyValues* pKV = new KeyValues("infractions");
	KeyValues::AutoDelete autoDelete(pKV);

	if (!pKV->LoadFromFile(g_pFullFileSystem, "addons/cs2scrim/data/infractions.txt"))
	{
		Warning("Failed to load addons/cs2scrim/data/infractions.txt\n");
		return;
	}

	for (KeyValues* pKey = pKV->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey())
	{
		uint64 iSteamId = pKey->GetUint64("steam", -1);
		int iEndTime = pKey->GetInt("endtime", -1);
		int iType = pKey->GetInt("type", -1);

		if (iSteamId == -1)
		{
			Warning("Infraction entry is missing 'steam' key\n");
			return;
		}

		if (iEndTime == -1)
		{
			Warning("Infraction entry is missing 'endtime' key\n");
			return;
		}

		if (iType == -1)
		{
			Warning("Infraction entry is missing 'type' key\n");
			return;
		}

		if (iType == CInfractionBase::Ban)
		{
			AddInfraction(new CBanInfraction(iEndTime, iSteamId));
		}
		else if (iType == CInfractionBase::Mute)
		{
			AddInfraction(new CMuteInfraction(iEndTime, iSteamId));
		}
		return;
	}
}

void CAdminSystem::SaveInfractions()
{
	KeyValues* pKV = new KeyValues("infractions");
	KeyValues* pSubKey;
	KeyValues::AutoDelete autoDelete(pKV);

	FOR_EACH_VEC(m_vecInfractions, i)
	{
		int timestamp = m_vecInfractions[i]->GetTimestamp();
		if (timestamp != 0 && timestamp < std::time(0))
			continue;

		char buf[5];
		V_snprintf(buf, sizeof(buf), "%d", i);
		pSubKey = new KeyValues(buf);
		pSubKey->AddUint64("steamid", m_vecInfractions[i]->GetSteamId64());
		pSubKey->AddInt("endtime", m_vecInfractions[i]->GetTimestamp());
		pSubKey->AddInt("type", m_vecInfractions[i]->GetType());

		pKV->AddSubKey(pSubKey);
	}

	pKV->SaveToFile(g_pFullFileSystem, "addons/cs2scrim/data/infractions.txt");
}

void CAdminSystem::AddInfraction(CInfractionBase* infraction)
{
	m_vecInfractions.AddToTail(infraction);
}

void CAdminSystem::ApplyInfractions(ZEPlayer *player)
{
	player->SetMuted(false);

	FOR_EACH_VEC(m_vecInfractions, i)
	{
		int timestamp = m_vecInfractions[i]->GetTimestamp();
		if (timestamp != 0 && timestamp <= std::time(0))
		{
			m_vecInfractions.Remove(i);
			continue;
		}

		if (m_vecInfractions[i]->GetSteamId64() == player->GetSteamId64())
			m_vecInfractions[i]->ApplyInfraction(player);
	}
}

CAdmin *CAdminSystem::FindAdmin(uint64 iSteamID)
{
	FOR_EACH_VEC(m_vecAdmins, i)
	{
		if (m_vecAdmins[i].GetSteamID() == iSteamID)
			return &m_vecAdmins[i];
	}

	return nullptr;
}

uint64 CAdminSystem::ParseFlags(const char* pszFlags)
{
	uint64 flags = 0;
	size_t length = V_strlen(pszFlags);

	for (size_t i = 0; i < length; i++)
	{
		char c = tolower(pszFlags[i]);
		if (c < 'a' || c > 'z')
			continue;

		if (c == 'z')
			return -1; // all flags

		flags |= ((uint64)1 << (c - 'a'));
	}

	return flags;
}

void CBanInfraction::ApplyInfraction(ZEPlayer *player)
{
	g_pEngineServer2->DisconnectClient(player->GetPlayerSlot().Get(), NETWORK_DISCONNECT_REJECT_BANNED); // "Kicked and banned"
}

void CMuteInfraction::ApplyInfraction(ZEPlayer* player)
{
	player->SetMuted(true);
}
