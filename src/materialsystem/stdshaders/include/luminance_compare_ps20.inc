#include "shaderlib/cshader.h"
class luminance_compare_ps20_Static_Index
{
public:
    luminance_compare_ps20_Static_Index( )
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
#define shaderStaticTest_luminance_compare_ps20 0
class luminance_compare_ps20_Dynamic_Index
{
public:
    luminance_compare_ps20_Dynamic_Index()
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
#define shaderDynamicTest_luminance_compare_ps20 0
