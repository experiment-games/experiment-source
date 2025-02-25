#pragma once

class CJsonToKeyValues
{
    public:
    CJsonToKeyValues(){};
    ~CJsonToKeyValues(){};

    // Converts an input char buffer JSON object to keyvalues.
    // Does NOT memory manage either input nor output!
    static bool ConvertJsonToKeyValues( const char *pInput, KeyValues *pOut );
    static void ConvertKeyValuesToJson( KeyValues *pKV, char *pOut, int nMaxLen );
};
