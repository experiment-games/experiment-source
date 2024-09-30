#include <cbase.h>
#include <tier3/tier3.h>
#include <datacache/imdlcache.h>

void ConnectCustomTier3Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount )
{
    AssertMsg ( !g_pMDLCache, "ConnectCustomTier3Libraries must be called before ConnectTier3Libraries!" );

    for ( int i = 0; i < nFactoryCount; ++i )
    {
        g_pMDLCache = mdlcache = ( IMDLCache * )pFactoryList[i]( MDLCACHE_INTERFACE_VERSION, NULL );
    }
}
