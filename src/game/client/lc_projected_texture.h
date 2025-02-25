#ifndef LC_PROJECTED_TEXTURE_H
#define LC_PROJECTED_TEXTURE_H

#ifdef _WIN32
#pragma once
#endif

/*
** type for ProjectedTexture functions, based on C_EnvProjectedTexture
*/
class LProjectedTexture
{
   public:
    LUA_DECLARE_SINGLE_LUA_INSTANCE( LProjectedTexture, LUA_PROJECTEDTEXTUREMETANAME );

    LProjectedTexture();
    ~LProjectedTexture();

    void ShutDownLightHandle( void );
    void ForceUpdateLight();

    virtual void SetupRefTable( lua_State *L );
    int m_nTableReference;

   public:
    Vector GetAbsOrigin() const
    {
        return m_vecOrigin;
    }

    void SetAbsOrigin( const Vector &vecOrigin )
    {
        m_vecOrigin = vecOrigin;
    }

    QAngle GetAbsAngles() const
    {
        return m_angRotation;
    }

    void SetAbsAngles( const QAngle &angRotation )
    {
        m_angRotation = angRotation;
    }

    // private: // Should probably make nice accessors, but let's keep it simple for now
    ClientShadowHandle_t m_LightHandle;
    FlashlightState_t m_FlashlightState;

    EHANDLE m_hTargetEntity;
    bool m_bLightWorld;

    Vector m_vecOrigin;
    QAngle m_angRotation;
    bool m_bIsValid;
};
typedef LProjectedTexture lua_ProjectedTexture;

/*
** access functions (stack -> C)
*/

LUA_API lua_ProjectedTexture *lua_toprojectedtexture( lua_State *L, int idx );

/*
** push functions (C -> stack)
*/
LUALIB_API lua_ProjectedTexture *luaL_checkprojectedtexture( lua_State *L, int narg );
LUALIB_API lua_ProjectedTexture *luaL_optprojectedtexture( lua_State *L, int narg, lua_ProjectedTexture *def );

#endif  // LC_PROJECTED_TEXTURE_H
