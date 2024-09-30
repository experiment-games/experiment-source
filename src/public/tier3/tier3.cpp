#include <cbase.h>
#include <tier3/tier3.h>
#include <datacache/imdlcache.h>
#include <istudiorender.h>

void ConnectCustomTier3Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount )
{
    AssertMsg( !g_pMDLCache && !mdlcache, "IMDLCache already loaded! ConnectCustomTier3Libraries must be called before ConnectTier3Libraries!" );
    AssertMsg( !g_pStudioRender && !studiorender, "IStudioRender already loaded! ConnectCustomTier3Libraries must be called before ConnectTier3Libraries!" );

    for ( int i = 0; i < nFactoryCount; ++i )
    {
        if ( !g_pMDLCache )
        {
            g_pMDLCache = mdlcache = ( IMDLCache * )pFactoryList[i]( MDLCACHE_INTERFACE_VERSION, NULL );
        }

        if ( !g_pStudioRender )
        {
            g_pStudioRender = studiorender = ( IStudioRender * )pFactoryList[i]( STUDIO_RENDER_INTERFACE_VERSION, NULL );
        }
    }
}
