#ifndef BASSMANAGER_H
#define BASSMANAGER_H
#pragma once

#include "GameEventListener.h"
#include "bass.h"

// since bass.h includes winlite we need to undef this
#ifdef CreateEvent
#undef CreateEvent
#endif

typedef void( CALLBACK CBassManagerCallback )( const void *buffer, DWORD length, void *user );

namespace BassManagerFlags
{
enum Type
{
    NONE = 0,
    SAMPLE_3D = 1 << 0,
    SAMPLE_MONO = 1 << 1,
    SAMPLE_LOOP = 1 << 2,
    DONT_PLAY = 1 << 3,
    STREAM_BLOCK = 1 << 4,
};
}

class CBassManager : public CBaseGameSystemPerFrame
{
   public:
    virtual bool Init();
    virtual void Shutdown();

    virtual void Update( float frametime );

   public:
    bool IsInitialized() const
    {
        return m_bInitialized;
    }
    void PlayUrl( const char *url, int flags = 0 );
    void PlayUrlEx( const char *url, int flags, CBassManagerCallback callback, void *callbackData );

   protected:
    int ToBassFlags( int flags );

    bool m_bInitialized;
    CUtlVector<HSTREAM> m_Streams;
};

extern CBassManager *g_pBassManager;

#endif  // BASSMANAGER_H
