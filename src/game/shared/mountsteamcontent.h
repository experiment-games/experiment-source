//========== Copyleft © 2011, Team Sandbox, Some rights reserved. ===========//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#ifndef MOUNTSTEAMCONTENT_H
#define MOUNTSTEAMCONTENT_H
#ifdef _WIN32
#pragma once
#endif

struct mountableGame_t
{
    // Directory inside the game's root directory where the game's content is stored
    // e.g: garrysmod, cstrike, hl2
    const char *directoryName;

    // The Steam AppID of the game
    int appId;

    // The human-readable name of the game
    const char *name;

    // Paths to VPK files that should be mounted
    CUtlVector< const char * > vpks;

    bool isMounted;
    bool isOwned;
    bool isInstalled;

    mountableGame_t( const char *gameName, const char *directoryName, int gameId, bool isInstalled, bool isOwned )
        : name( gameName ),
        directoryName( directoryName ),
        appId( gameId ),
        isMounted( false ),
        isOwned( isOwned ),
        isInstalled( isInstalled )
    {
    }

    mountableGame_t( const mountableGame_t &other )
        : name( other.name ),
        directoryName( other.directoryName ),
        appId( other.appId ),
        isMounted( other.isMounted ),
        isOwned( other.isOwned ),
        isInstalled( other.isInstalled )
    {
        for ( int i = 0; i < other.vpks.Count(); i++ )
        {
            vpks.AddToTail( other.vpks[i] );
        }
    }

    void AddVPK( const char *vpkPath )
    {
        vpks.AddToTail( vpkPath );
    }
};

void SetupRootSearchPaths( const char *gamePath, lua_State *L );
void RemoveRootSearchPaths( const char *gamePath, lua_State *L );
void InitializeGameContentMounting();
bool MountGameContentByAppId( int nAppId );
bool UnmountGameContentByAppId( int nAppId );
CUtlVector< mountableGame_t > &GetMountableGames();

#endif  // MOUNTSTEAMCONTENT_H
