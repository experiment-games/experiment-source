//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include <cbase.h>
#include "vgui/iinput.h"
#include "vgui_controls/Controls.h"
#include <luamanager.h>
#include "luasrclib.h"
#include <mathlib/lvector.h>

#include "scripted_controls/lPanel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

static int input_CandidateListStartsAtOne( lua_State *L )
{
    lua_pushboolean( L, input()->CandidateListStartsAtOne() );
    return 1;
}

static int input_GetAppModalSurface( lua_State *L )
{
    Panel::PushVPanelLuaInstance( L, input()->GetAppModalSurface() );
    return 1;
}

static int input_GetCandidateListCount( lua_State *L )
{
    lua_pushinteger( L, input()->GetCandidateListCount() );
    return 1;
}

static int input_GetCandidateListPageSize( lua_State *L )
{
    lua_pushinteger( L, input()->GetCandidateListPageSize() );
    return 1;
}

static int input_GetCandidateListPageStart( lua_State *L )
{
    lua_pushinteger( L, input()->GetCandidateListPageStart() );
    return 1;
}

static int input_GetCandidateListSelectedItem( lua_State *L )
{
    lua_pushinteger( L, input()->GetCandidateListSelectedItem() );
    return 1;
}

static int input_GetCurrentIMEHandle( lua_State *L )
{
    lua_pushinteger( L, input()->GetCurrentIMEHandle() );
    return 1;
}

static int input_GetCursorPos__USE_VCR_MODE( lua_State *L )
{
    int x, y;
    input()->GetCursorPos__USE_VCR_MODE( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}

static int input_GetCursorPosition( lua_State *L )
{
    int x, y;
    input()->GetCursorPosition( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}

static int input_GetEnglishIMEHandle( lua_State *L )
{
    lua_pushinteger( L, input()->GetEnglishIMEHandle() );
    return 1;
}

static int input_GetFocus( lua_State *L )
{
    Panel::PushVPanelLuaInstance( L, input()->GetFocus() );
    return 1;
}

static int input_GetModalSubTree( lua_State *L )
{
    Panel::PushVPanelLuaInstance( L, input()->GetModalSubTree() );
    return 1;
}

static int input_GetMouseCapture( lua_State *L )
{
    Panel::PushVPanelLuaInstance( L, input()->GetMouseCapture() );
    return 1;
}

static int input_GetMouseOver( lua_State *L )
{
    Panel::PushVPanelLuaInstance( L, input()->GetMouseOver() );
    return 1;
}

static int input_GetShouldInvertCompositionString( lua_State *L )
{
    lua_pushboolean( L, input()->GetShouldInvertCompositionString() );
    return 1;
}

static int input_IsKeyDown( lua_State *L )
{
    lua_pushboolean( L, input()->IsKeyDown( ( KeyCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int input_IsMouseDown( lua_State *L )
{
    lua_pushboolean( L, input()->IsMouseDown( ( KeyCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int engine_Key_LookupBinding( lua_State *L )
{
    bool exact = lua_toboolean( L, 2 );

    if ( exact )
        lua_pushstring( L, engine->Key_LookupBindingExact( luaL_checkstring( L, 1 ) ) );
    else
        lua_pushstring( L, engine->Key_LookupBinding( luaL_checkstring( L, 1 ) ) );

    return 1;
}

static int engine_Key_BindingForKey( lua_State *L )
{
    ButtonCode_t code = ( ButtonCode_t )( int )luaL_checknumber( L, 1 );
    const char *binding = engine->Key_BindingForKey( code );

    // Dunno why binding returns 0x10000 for BUTTON_CODE_LAST, but lets catch that
    if ( binding && ( ( uintptr_t )binding != 0x10000 ) )
        lua_pushstring( L, binding );
    else
        lua_pushstring( L, "" );

    return 1;
}

static int input_OnChangeIME( lua_State *L )
{
    input()->OnChangeIME( luaL_checkboolean( L, 1 ) );
    return 0;
}

static int input_OnChangeIMEByHandle( lua_State *L )
{
    input()->OnChangeIMEByHandle( luaL_checknumber( L, 1 ) );
    return 0;
}

static int input_OnChangeIMEConversionModeByHandle( lua_State *L )
{
    input()->OnChangeIMEConversionModeByHandle( luaL_checknumber( L, 1 ) );
    return 0;
}

static int input_OnChangeIMESentenceModeByHandle( lua_State *L )
{
    input()->OnChangeIMESentenceModeByHandle( luaL_checknumber( L, 1 ) );
    return 0;
}

static int input_OnIMEChangeCandidates( lua_State *L )
{
    input()->OnIMEChangeCandidates();
    return 0;
}

static int input_OnIMECloseCandidates( lua_State *L )
{
    input()->OnIMECloseCandidates();
    return 0;
}

static int input_OnIMEComposition( lua_State *L )
{
    input()->OnIMEComposition( luaL_checknumber( L, 1 ) );
    return 0;
}

static int input_OnIMEEndComposition( lua_State *L )
{
    input()->OnIMEEndComposition();
    return 0;
}

static int input_OnIMERecomputeModes( lua_State *L )
{
    input()->OnIMERecomputeModes();
    return 0;
}

static int input_OnIMEShowCandidates( lua_State *L )
{
    input()->OnIMEShowCandidates();
    return 0;
}

static int input_OnIMEStartComposition( lua_State *L )
{
    input()->OnIMEStartComposition();
    return 0;
}

static int input_OnInputLanguageChanged( lua_State *L )
{
    input()->OnInputLanguageChanged();
    return 0;
}

static int input_OnKeyCodeUnhandled( lua_State *L )
{
    input()->OnKeyCodeUnhandled( luaL_checknumber( L, 1 ) );
    return 0;
}

static int input_RegisterKeyCodeUnhandledListener( lua_State *L )
{
    input()->RegisterKeyCodeUnhandledListener( luaL_checkvpanel( L, 1 ) );
    return 0;
}

static int input_ReleaseAppModalSurface( lua_State *L )
{
    input()->ReleaseAppModalSurface();
    return 0;
}

static int input_ReleaseModalSubTree( lua_State *L )
{
    input()->ReleaseModalSubTree();
    return 0;
}

static int input_ScreenToWorld( lua_State *L )
{
    CBasePlayer *pPlayer = CBasePlayer::GetLocalPlayer();
    float fov = pPlayer->GetFOV();
    QAngle vecRenderAngles = pPlayer->GetRenderAngles();
    float x = luaL_checknumber( L, 1 );
    float y = luaL_checknumber( L, 2 );

    Vector vecPickingRay;
    ScreenToWorld( x, y, fov, vecRenderAngles, vecPickingRay );
    lua_pushvector( L, vecPickingRay );

    return 1;
}

static int input_AimToVector( lua_State *L )
{
    QAngle vecAngles = luaL_checkangle( L, 1 );
    float fov = luaL_checknumber( L, 2 );
    float x = luaL_checknumber( L, 3 );
    float y = luaL_checknumber( L, 4 );
    float nScreenWidth = luaL_checknumber( L, 5 );
    float nScreenHeight = luaL_checknumber( L, 6 );

    Vector vecPickingRay;
    ScreenToWorld( x, y, fov, vecAngles, vecPickingRay, nScreenWidth, nScreenHeight );
    lua_pushvector( L, vecPickingRay );

    return 1;
}

static int input_SetAppModalSurface( lua_State *L )
{
    input()->SetAppModalSurface( luaL_checkvpanel( L, 1 ) );
    return 0;
}

static int input_SetCandidateListPageStart( lua_State *L )
{
    input()->SetCandidateListPageStart( luaL_checknumber( L, 1 ) );
    return 0;
}

static int input_SetCandidateWindowPos( lua_State *L )
{
    input()->SetCandidateWindowPos( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
    return 0;
}

static int input_SetCursorPos( lua_State *L )
{
    input()->SetCursorPos( luaL_checknumber( L, 1 ), luaL_checknumber( L, 2 ) );
    return 0;
}

static int input_SetModalSubTree( lua_State *L )
{
    input()->SetModalSubTree( luaL_checkvpanel( L, 1 ), luaL_checkvpanel( L, 2 ), luaL_optboolean( L, 3, true ) );
    return 0;
}

static int input_SetModalSubTreeReceiveMessages( lua_State *L )
{
    input()->SetModalSubTreeReceiveMessages( luaL_checkboolean( L, 1 ) );
    return 0;
}

static int input_SetMouseCapture( lua_State *L )
{
    input()->SetMouseCapture( luaL_optvpanel( L, 1, NULL ) );
    return 0;
}

static int input_SetMouseCaptureEx( lua_State *L )
{
    input()->SetMouseCaptureEx( luaL_checkvpanel( L, 1 ), ( MouseCode )( int )luaL_checknumber( L, 2 ) );
    return 0;
}

static int input_SetMouseFocus( lua_State *L )
{
    input()->SetMouseFocus( luaL_checkvpanel( L, 1 ) );
    return 0;
}

static int input_ShouldModalSubTreeReceiveMessages( lua_State *L )
{
    lua_pushboolean( L, input()->ShouldModalSubTreeReceiveMessages() );
    return 1;
}

static int input_UnregisterKeyCodeUnhandledListener( lua_State *L )
{
    input()->UnregisterKeyCodeUnhandledListener( luaL_checkvpanel( L, 1 ) );
    return 0;
}

static int input_WasKeyPressed( lua_State *L )
{
    lua_pushboolean( L, input()->WasKeyPressed( ( KeyCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int input_WasKeyReleased( lua_State *L )
{
    lua_pushboolean( L, input()->WasKeyReleased( ( KeyCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int input_WasKeyTyped( lua_State *L )
{
    lua_pushboolean( L, input()->WasKeyTyped( ( KeyCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int input_WasMouseDoublePressed( lua_State *L )
{
    lua_pushboolean( L, input()->WasMouseDoublePressed( ( MouseCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int input_WasMousePressed( lua_State *L )
{
    lua_pushboolean( L, input()->WasMousePressed( ( MouseCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static int input_WasMouseReleased( lua_State *L )
{
    lua_pushboolean( L, input()->WasMouseReleased( ( MouseCode )( int )luaL_checknumber( L, 1 ) ) );
    return 1;
}

static const luaL_Reg inputlib[] = {
    { "CandidateListStartsAtOne", input_CandidateListStartsAtOne },
    { "GetAppModalSurface", input_GetAppModalSurface },
    { "GetCandidateListCount", input_GetCandidateListCount },
    { "GetCandidateListPageSize", input_GetCandidateListPageSize },
    { "GetCandidateListPageStart", input_GetCandidateListPageStart },
    { "GetCandidateListSelectedItem", input_GetCandidateListSelectedItem },
    { "GetCurrentIMEHandle", input_GetCurrentIMEHandle },
    { "GetCursorPos__USE_VCR_MODE", input_GetCursorPos__USE_VCR_MODE },
    { "GetCursorPos", input_GetCursorPos__USE_VCR_MODE },
    { "GetCursorPosition", input_GetCursorPosition },
    { "GetEnglishIMEHandle", input_GetEnglishIMEHandle },
    { "GetFocus", input_GetFocus },
    { "GetModalSubTree", input_GetModalSubTree },
    { "GetMouseCapture", input_GetMouseCapture },
    { "GetMouseOver", input_GetMouseOver },
    { "GetShouldInvertCompositionString", input_GetShouldInvertCompositionString },
    { "IsKeyDown", input_IsKeyDown },
    { "IsMouseDown", input_IsMouseDown },
    { "LookupBinding", engine_Key_LookupBinding },
    { "LookupKeyBinding", engine_Key_BindingForKey },
    { "OnChangeIME", input_OnChangeIME },
    { "OnChangeIMEByHandle", input_OnChangeIMEByHandle },
    { "OnChangeIMEConversionModeByHandle", input_OnChangeIMEConversionModeByHandle },
    { "OnChangeIMESentenceModeByHandle", input_OnChangeIMESentenceModeByHandle },
    { "OnIMEChangeCandidates", input_OnIMEChangeCandidates },
    { "OnIMECloseCandidates", input_OnIMECloseCandidates },
    { "OnIMEComposition", input_OnIMEComposition },
    { "OnIMEEndComposition", input_OnIMEEndComposition },
    { "OnIMERecomputeModes", input_OnIMERecomputeModes },
    { "OnIMEShowCandidates", input_OnIMEShowCandidates },
    { "OnIMEStartComposition", input_OnIMEStartComposition },
    { "OnInputLanguageChanged", input_OnInputLanguageChanged },
    { "OnKeyCodeUnhandled", input_OnKeyCodeUnhandled },
    { "RegisterKeyCodeUnhandledListener", input_RegisterKeyCodeUnhandledListener },
    { "ReleaseAppModalSurface", input_ReleaseAppModalSurface },
    { "ReleaseModalSubTree", input_ReleaseModalSubTree },
    { "ScreenToWorld", input_ScreenToWorld },
    { "AimToVector", input_AimToVector },
    { "SetAppModalSurface", input_SetAppModalSurface },
    { "SetCandidateListPageStart", input_SetCandidateListPageStart },
    { "SetCandidateWindowPos", input_SetCandidateWindowPos },
    { "SetCursorPosition", input_SetCursorPos },
    { "SetModalSubTree", input_SetModalSubTree },
    { "SetModalSubTreeReceiveMessages", input_SetModalSubTreeReceiveMessages },
    { "SetMouseCapture", input_SetMouseCapture },
    { "SetMouseCaptureEx", input_SetMouseCaptureEx },
    { "SetMouseFocus", input_SetMouseFocus },
    { "ShouldModalSubTreeReceiveMessages", input_ShouldModalSubTreeReceiveMessages },
    { "UnregisterKeyCodeUnhandledListener", input_UnregisterKeyCodeUnhandledListener },
    { "WasKeyPressed", input_WasKeyPressed },
    { "WasKeyReleased", input_WasKeyReleased },
    { "WasKeyTyped", input_WasKeyTyped },
    { "WasMouseDoublePressed", input_WasMouseDoublePressed },
    { "WasMousePressed", input_WasMousePressed },
    { "WasMouseReleased", input_WasMouseReleased },
    { NULL, NULL } };

/*
** Open input library
*/
LUALIB_API int luaopen_input( lua_State *L )
{
    luaL_register( L, LUA_INPUTLIBNAME, inputlib );
    return 1;
}
