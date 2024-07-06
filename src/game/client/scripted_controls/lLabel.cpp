#include <cbase.h>

#include <vgui_int.h>
#include <luamanager.h>
#include "luasrclib.h"
#include <lColor.h>

#include <scripted_controls/lLabel.h>
#include "scripted_controls/lPanel.h"
#include <scripted_controls/lButton.h>
#include <vgui/LVGUI.h>
#include <LKeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
LLabel::LLabel( Panel *parent, const char *panelName, const char *text, lua_State *L )
    : Label( parent, panelName, text )
{
    m_lua_State = L;
}

void LLabel::PushPanelToLua( lua_State *L )
{
    lua_pushlabel( L, this );
}

/*
** access functions (stack -> C)
*/

LUA_API lua_Label *lua_tolabel( lua_State *L, int idx )
{
    PHandle *phPanel =
        dynamic_cast< PHandle * >( ( PHandle * )lua_touserdata( L, idx ) );
    if ( phPanel == NULL )
        return NULL;
    return dynamic_cast< lua_Label * >( phPanel->Get() );
}

/*
** push functions (C -> stack)
*/

LUA_API void lua_pushlabel( lua_State *L, Label *pLabel )
{
    LLabel *plLabel = dynamic_cast< LLabel * >( pLabel );
    if ( plLabel )
        ++plLabel->m_nRefCount;
    PHandle *phPanel = ( PHandle * )lua_newuserdata( L, sizeof( PHandle ) );
    phPanel->Set( pLabel );
    luaL_getmetatable( L, "Label" );
    lua_setmetatable( L, -2 );
}

LUALIB_API lua_Label *luaL_checklabel( lua_State *L, int narg )
{
    lua_Label *d = lua_tolabel( L, narg );
    if ( d == NULL ) /* avoid extra test when d is not 0 */
        luaL_argerror( L, narg, "Label expected, got INVALID_PANEL" );
    return d;
}

// virtual void GetContentSize( int &wide, int &tall );
// virtual void SetContentAlignment( Alignment alignment );
// virtual void SetEnabled( bool state );
// virtual void SetTextInset( int xInset, int yInset );
// virtual void GetTextInset( int *xInset, int *yInset );
// virtual void SetFgColor( Color color );
// virtual Color GetFgColor();
// virtual void SetDisabledFgColor1( Color color );
// virtual void SetDisabledFgColor2( Color color );
// virtual Color GetDisabledFgColor1();
// virtual Color GetDisabledFgColor2();
// virtual void SetTextColorState( EColorState state );
// virtual void SetFont( HFont font );
// virtual HFont GetFont();
// virtual Panel *HasHotkey( wchar_t key );
// virtual void SetHotkey( wchar_t key );
// virtual wchar_t GetHotKey();
// virtual void SetAssociatedControl( Panel *control );
// virtual int AddImage( IImage *image, int preOffset );  // Return the index the image was placed in
// virtual void SetImageAtIndex( int index, IImage *image, int preOffset );
// virtual void SetImagePreOffset( int index, int preOffset );  // Set the offset in pixels before the image
// virtual IImage *GetImageAtIndex( int index );
// virtual int GetImageCount();
// virtual void ClearImages();
// virtual void ResetToSimpleTextImage();
// virtual void SetImageBounds( int index, int x, int width );
// virtual TextImage *GetTextImage();
// virtual int SetTextImageIndex( int newIndex );
// virtual bool RequestInfo( KeyValues *outputData );
// virtual void SizeToContents();

static int Label_ChainToAnimationMap( lua_State *L )
{
    luaL_checklabel( L, 1 )->ChainToAnimationMap();
    return 0;
}

static int Label_ChainToMap( lua_State *L )
{
    luaL_checklabel( L, 1 )->ChainToMap();
    return 0;
}

static int Label_GetPanelBaseClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checklabel( L, 1 )->GetPanelBaseClassName() );
    return 1;
}

static int Label_GetPanelClassName( lua_State *L )
{
    lua_pushstring( L, luaL_checklabel( L, 1 )->GetPanelClassName() );
    return 1;
}

static int Label_GetRefTable( lua_State *L )
{
    LLabel *plLabel = dynamic_cast< LLabel * >( luaL_checklabel( L, 1 ) );
    if ( plLabel )
    {
        if ( !lua_isrefvalid( L, plLabel->m_nTableReference ) )
        {
            lua_newtable( L );
            plLabel->m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );
        }
        lua_getref( L, plLabel->m_nTableReference );
    }
    else
        lua_pushnil( L );
    return 1;
}

static int Label_KB_AddBoundKey( lua_State *L )
{
    luaL_checklabel( L, 1 )->KB_AddBoundKey(
        luaL_checkstring( L, 2 ), luaL_checkint( L, 3 ), luaL_checkint( L, 4 ) );
    return 0;
}

static int Label_KB_ChainToMap( lua_State *L )
{
    luaL_checklabel( L, 1 )->KB_ChainToMap();
    return 0;
}

static int Label_OnCursorEntered( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnCursorEntered();
    return 0;
}

static int Label_OnCursorExited( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnCursorExited();
    return 0;
}

static int Label_OnKillFocus( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnKillFocus();
    return 0;
}

static int Label_OnSetFocus( lua_State *L )
{
    luaL_checklabel( L, 1 )->OnSetFocus();
    return 0;
}

static int Label_PerformLayout( lua_State *L )
{
    luaL_checklabel( L, 1 )->PerformLayout();
    return 0;
}

static int Label_SizeToContents( lua_State *L )
{
    luaL_checklabel( L, 1 )->SizeToContents();
    return 0;
}

static int Label_SetContentAlignment( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetContentAlignment( (vgui::Label::Alignment) luaL_checkint( L, 2 ) );
    return 0;
}

static int Label_SetFont( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetFont( luaL_checkfont( L, 2 ) );
    return 0;
}

static int Label_GetFont( lua_State *L )
{
    lua_pushfont( L, luaL_checklabel( L, 1 )->GetFont() );
    return 1;
}

static int Label_SetFgColor( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetFgColor( luaL_checkcolor( L, 2 ) );
    return 0;
}

static int Label_GetFgColor( lua_State *L )
{
    lua_pushcolor( L, luaL_checklabel( L, 1 )->GetFgColor() );
    return 1;
}

static int Label_SetText( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetText( luaL_checkstring( L, 2 ) );
    return 0;
}

static int Label_GetTextInset( lua_State *L )
{
    int xInset, yInset;
    luaL_checklabel( L, 1 )->GetTextInset( &xInset, &yInset );
    lua_pushinteger( L, xInset );
    lua_pushinteger( L, yInset );
    return 2;
}

static int Label_SetTextInset( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetTextInset( luaL_checkint( L, 2 ), luaL_checkint( L, 3 ) );
    return 0;
}

static int Label_GetHotKey( lua_State *L )
{
    lua_pushinteger( L, luaL_checklabel( L, 1 )->GetHotKey() );
    return 1;
}

static int Label_HasHotkey( lua_State *L )
{
    lua_pushpanel( L, luaL_checklabel( L, 1 )->HasHotkey( luaL_checkint( L, 2 ) ) );
    return 1;
}

static int Label_SetHotkey( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetHotkey( luaL_checkint( L, 2 ) );
    return 0;
}

static int Label_SetAssociatedControl( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetAssociatedControl( luaL_checkpanel( L, 2 ) );
    return 0;
}

// TODO: Implement luaL_checkimage etc. (low priority, used nowhere)
//static int Label_AddImage( lua_State *L )
//{
//    luaL_checklabel( L, 1 )->AddImage( luaL_checkimage( L, 2 ), luaL_checkint( L, 3 ) );
//    return 0;
//}
//
//static int Label_SetImageAtIndex( lua_State *L )
//{
//    luaL_checklabel( L, 1 )->SetImageAtIndex( luaL_checkint( L, 2 ), luaL_checkimage( L, 3 ), luaL_checkint( L, 4 ) );
//    return 0;
//}

static int Label_SetImagePreOffset( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetImagePreOffset( luaL_checkint( L, 2 ), luaL_checkint( L, 3 ) );
    return 0;
}

//static int Label_GetImageAtIndex( lua_State *L )
//{
//    lua_pushimage( L, luaL_checklabel( L, 1 )->GetImageAtIndex( luaL_checkint( L, 2 ) ) );
//    return 1;
//}

static int Label_GetImageCount( lua_State *L )
{
    lua_pushinteger( L, luaL_checklabel( L, 1 )->GetImageCount() );
    return 1;
}

static int Label_ClearImages( lua_State *L )
{
    luaL_checklabel( L, 1 )->ClearImages();
    return 0;
}

static int Label_ResetToSimpleTextImage( lua_State *L )
{
    luaL_checklabel( L, 1 )->ResetToSimpleTextImage();
    return 0;
}

static int Label_SetImageBounds( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetImageBounds( luaL_checkint( L, 2 ), luaL_checkint( L, 3 ), luaL_checkint( L, 4 ) );
    return 0;
}

//static int Label_GetTextImage( lua_State *L )
//{
//    lua_pushimage( L, luaL_checklabel( L, 1 )->GetTextImage() );
//    return 1;
//}

static int Label_SetTextImageIndex( lua_State *L )
{
    lua_pushinteger( L, luaL_checklabel( L, 1 )->SetTextImageIndex( luaL_checkint( L, 2 ) ) );
    return 1;
}

static int Label_SetWrap( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetWrap( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int Label_SetCenterWrap( lua_State *L )
{
    luaL_checklabel( L, 1 )->SetCenterWrap( luaL_checkboolean( L, 2 ) );
    return 0;
}

static int Label_RequestInfo( lua_State *L )
{
    lua_pushboolean( L, luaL_checklabel( L, 1 )->RequestInfo( luaL_checkkeyvalues( L, 2 ) ) );
    return 1;
}

static int Label___index( lua_State *L )
{
    Label *pLabel = lua_tolabel( L, 1 );
    if ( pLabel == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }
    LLabel *plLabel = dynamic_cast< LLabel * >( pLabel );
    if ( plLabel && lua_isrefvalid( L, plLabel->m_nTableReference ) )
    {
        lua_getref( L, plLabel->m_nTableReference );
        lua_pushvalue( L, 2 );
        lua_gettable( L, -2 );
        if ( lua_isnil( L, -1 ) )
        {
            lua_pop( L, 2 );
            lua_getmetatable( L, 1 );
            lua_pushvalue( L, 2 );
            lua_gettable( L, -2 );
            if ( lua_isnil( L, -1 ) )
            {
                lua_pop( L, 2 );
                luaL_getmetatable( L, "Panel" );
                lua_pushvalue( L, 2 );
                lua_gettable( L, -2 );
            }
        }
    }
    else
    {
        lua_getmetatable( L, 1 );
        lua_pushvalue( L, 2 );
        lua_gettable( L, -2 );
        if ( lua_isnil( L, -1 ) )
        {
            lua_pop( L, 2 );
            luaL_getmetatable( L, "Panel" );
            lua_pushvalue( L, 2 );
            lua_gettable( L, -2 );
        }
    }
    return 1;
}

static int Label___newindex( lua_State *L )
{
    Label *pLabel = lua_tolabel( L, 1 );

    if ( pLabel == NULL )
    { /* avoid extra test when d is not 0 */
        lua_Debug ar1;
        lua_getstack( L, 1, &ar1 );
        lua_getinfo( L, "fl", &ar1 );
        lua_Debug ar2;
        lua_getinfo( L, ">S", &ar2 );
        lua_pushfstring( L, "%s:%d: attempt to index an INVALID_PANEL", ar2.short_src, ar1.currentline );
        return lua_error( L );
    }

    LLabel *plLabel = dynamic_cast< LLabel * >( pLabel );

    if ( plLabel )
    {
        if ( !lua_isrefvalid( L, plLabel->m_nTableReference ) )
        {
            lua_newtable( L );
            plLabel->m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );
        }
        lua_getref( L, plLabel->m_nTableReference );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
        lua_pop( L, 1 );
        return 0;
    }

    // Special case for LButtons
    // TODO: Somehow inherit instead, but multiple inheritance is tricky with our current setup
    // (LButton inherits from Button, which inherits from Panel, like LLabel -> Label -> Panel)
    // This is a quick fix for now
    LButton *plButton = dynamic_cast< LButton * >( pLabel );

    if ( plButton )
    {
        if ( !lua_isrefvalid( L, plButton->m_nTableReference ) )
        {
            lua_newtable( L );
            plButton->m_nTableReference = luaL_ref( L, LUA_REGISTRYINDEX );
        }
        lua_getref( L, plButton->m_nTableReference );
        lua_pushvalue( L, 3 );
        lua_setfield( L, -2, luaL_checkstring( L, 2 ) );
        lua_pop( L, 1 );
        return 0;
    }

    lua_Debug ar1;
    lua_getstack( L, 1, &ar1 );
    lua_getinfo( L, "fl", &ar1 );
    lua_Debug ar2;
    lua_getinfo( L, ">S", &ar2 );
    lua_pushfstring( L, "%s:%d: attempt to index a non-scripted panel", ar2.short_src, ar1.currentline );
    return lua_error( L );
}

static int Label___eq( lua_State *L )
{
    lua_pushboolean( L, lua_tolabel( L, 1 ) == lua_tolabel( L, 2 ) );
    return 1;
}

static int Label___tostring( lua_State *L )
{
    Label *pLabel = lua_tolabel( L, 1 );
    if ( pLabel == NULL )
        lua_pushstring( L, "INVALID_PANEL" );
    else
    {
        const char *pName = pLabel->GetName();
        if ( Q_strcmp( pName, "" ) == 0 )
            pName = "(no name)";
        lua_pushfstring( L, "Label: \"%s\"", pName );
    }
    return 1;
}

static const luaL_Reg Labelmeta[] = {
    { "ChainToAnimationMap", Label_ChainToAnimationMap },
    { "ChainToMap", Label_ChainToMap },
    { "GetPanelBaseClassName", Label_GetPanelBaseClassName },
    { "GetPanelClassName", Label_GetPanelClassName },
    { "GetRefTable", Label_GetRefTable },
    { "KB_AddBoundKey", Label_KB_AddBoundKey },
    { "KB_ChainToMap", Label_KB_ChainToMap },
    { "OnCursorEntered", Label_OnCursorEntered },
    { "OnCursorExited", Label_OnCursorExited },
    { "OnKillFocus", Label_OnKillFocus },
    { "OnSetFocus", Label_OnSetFocus },
    { "PerformLayout", Label_PerformLayout },
    { "SizeToContents", Label_SizeToContents },
    { "SetContentAlignment", Label_SetContentAlignment },
    { "SetText", Label_SetText },
    { "SetFont", Label_SetFont },
    { "GetFont", Label_GetFont },
    { "SetFgColor", Label_SetFgColor },
    { "GetFgColor", Label_GetFgColor },
    { "GetTextInset", Label_GetTextInset },
    { "SetTextInset", Label_SetTextInset },
    { "GetHotKey", Label_GetHotKey },
    { "HasHotkey", Label_HasHotkey },
    { "SetHotkey", Label_SetHotkey },
    { "SetAssociatedControl", Label_SetAssociatedControl },
    //{ "AddImage", Label_AddImage },
    //{ "SetImageAtIndex", Label_SetImageAtIndex },
    { "SetImagePreOffset", Label_SetImagePreOffset },
    //{ "GetImageAtIndex", Label_GetImageAtIndex },
    { "GetImageCount", Label_GetImageCount },
    { "ClearImages", Label_ClearImages },
    { "ResetToSimpleTextImage", Label_ResetToSimpleTextImage },
    { "SetImageBounds", Label_SetImageBounds },
    //{ "GetTextImage", Label_GetTextImage },
    { "SetTextImageIndex", Label_SetTextImageIndex },
    { "SetWrap", Label_SetWrap },
    { "SetCenterWrap", Label_SetCenterWrap },
    { "RequestInfo", Label_RequestInfo },
    { "__index", Label___index },
    { "__newindex", Label___newindex },
    { "__eq", Label___eq },
    { "__tostring", Label___tostring },
    { "__gc", Panel___gc },

    { NULL, NULL } };

static int luasrc_Label( lua_State *L )
{
    Label *pLabel =
        new LLabel( luaL_optpanel( L, 1, VGui_GetClientLuaRootPanel() ),
                    luaL_checkstring( L, 2 ),
                    luaL_optstring( L, 3, "Label" ),
                    L );
    lua_pushlabel( L, pLabel );
    return 1;
}

static const luaL_Reg Label_funcs[] = { { "Label", luasrc_Label },
                                        { NULL, NULL } };

/*
** Open Label object
*/
LUALIB_API int luaopen_vgui_Label( lua_State *L )
{
    luaL_newmetatable( L, "Label" );
    luaL_register( L, NULL, Labelmeta );
    lua_pushstring( L, LUA_PANELLIBNAME );
    lua_setfield( L, -2, "__type" ); /* metatable.__type = "Panel" */
    luaL_register( L, LUA_VGUILIBNAME, Label_funcs );
    lua_pop( L, 2 );
    return 0;
}
