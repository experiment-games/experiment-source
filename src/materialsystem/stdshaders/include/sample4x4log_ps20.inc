#include "shaderlib/cshader.h"
class sample4x4log_ps20_Static_Index
{
public:
    sample4x4log_ps20_Static_Index( )
    {
    }
    int GetIndex()
    {
        // Asserts to make sure that we aren't using any skipped combinations.
        // Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
        return 0;
    }
};
#define shaderStaticTest_sample4x4log_ps20 0
class sample4x4log_ps20_Dynamic_Index
{
public:
    sample4x4log_ps20_Dynamic_Index()
    {
    }
    int GetIndex()
    {
        // Asserts to make sure that we aren't using any skipped combinations.
        // Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
        return 0;
    }
};
#define shaderDynamicTest_sample4x4log_ps20 0
