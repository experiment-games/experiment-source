#include "cbase.h"
#include "vgui/isurface.h"
#include "vgui/ILocalize.h"
#include "vgui_controls/Controls.h"
#include "luamanager.h"
#include "luasrclib.h"
#include "vgui/lvgui.h"
#include <litexture.h>
#include <cpng.h>
#include "VGuiMatSurface/IMatSystemSurface.h"
#include <materialsystem/limaterial.h>
#include "filesystem.h"
#include "utlbuffer.h"
#include "materialsystem/itexture.h"
#include <materialsystem/imaterial.h>
#include <materialsystem/imaterialsystem.h>
#include "materialsystem/imaterialvar.h"
#include <bitmap/bitmap.h>
#include <cpng.h>
#include "vgui_controls/Controls.h"

#include "scripted_controls/lPanel.h"
#include <lColor.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

LUA_REGISTRATION_INIT( Surfaces )

LUA_BINDING_BEGIN( Surfaces, AddBitmapFontFile, "library", "Adds a bitmap font file to the list of fonts." )
{
    const char *fontFileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fontFileName" );
    lua_pushboolean( L, surface()->AddBitmapFontFile( fontFileName ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the font was added, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, AddCustomFontFile, "library", "Adds a custom font file to the list of fonts." )
{
    const char *fontFileName = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fontFileName" );
    const char *fontName = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "fontName" );
    lua_pushboolean( L, surface()->AddCustomFontFile( fontFileName, fontName ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the font was added, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, AddPanel, "library", "Adds a panel to the surface." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->AddPanel( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, ApplyChanges, "library", "Applies changes to the surface." )
{
    surface()->ApplyChanges();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, BringToFront, "library", "Brings the panel to the front." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->BringToFront( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, CalculateMouseVisible, "library", "Calculates if the mouse is visible." )
{
    surface()->CalculateMouseVisible();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, ClearTemporaryFontCache, "library", "Clears the temporary font cache." )
{
    surface()->ClearTemporaryFontCache();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, CreateFont, "library", "Creates a new font." )
{
    lua_pushfont( L, surface()->CreateFont() );
    return 1;
}
LUA_BINDING_END( "font", "The new font." )

LUA_BINDING_BEGIN( Surfaces, CreateNewTextureID, "library", "Creates a new texture ID." )
{
    bool procedural = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 1, false, "procedural" );

    lua_pushinteger( L, CreateNewAutoDestroyTextureId( procedural ) );

    return 1;
}
LUA_BINDING_END( "integer", "The new texture ID." )

LUA_BINDING_BEGIN( Surfaces, CreatePopup, "library", "Creates a popup." )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );

    bool isMinimized = LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "isMinimized" );
    bool shouldShowTaskbarIcon = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 3, true, "shouldShowTaskbarIcon" );
    bool isDisabled = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, false, "isDisabled" );
    bool hasMouseInput = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 5, true, "hasMouseInput" );
    bool hasKeyboardInput = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 6, true, "hasKeyboardInput" );

    surface()->CreatePopup( panel->GetVPanel(), isMinimized, shouldShowTaskbarIcon, isDisabled, hasMouseInput, hasKeyboardInput );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DisableClipping, "library", "Disables clipping" )
{
    bool bClippingDisabled;
    int left, top, right, bottom;
    g_pMatSystemSurface->GetClippingRect( left, top, right, bottom, bClippingDisabled );
    bool shouldDisable = LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "shouldDisable" );
    g_pMatSystemSurface->DisableClipping( shouldDisable );
    lua_pushboolean( L, bClippingDisabled );
    return 1;
}
LUA_BINDING_END( "boolean", "The previous clipping state" )

LUA_BINDING_BEGIN( Surfaces, DrawFilledRectangle, "library", "Draws a filled rectangle" )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    float width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "width" );
    float height = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "height" );

    surface()->DrawFilledRect( x, y, x + width, y + height );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawFilledRectangleFade, "library", "Draws a filled rectangle with a fade" )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    float width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "width" );
    float height = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "height" );
    int alpha1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "alpha1" );
    int alpha2 = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "alpha2" );
    bool horizontal = LUA_BINDING_ARGUMENT( luaL_checkboolean, 7, "horizontal" );

    surface()->DrawFilledRectFade( x, y, x + width, y + height, alpha1, alpha2, horizontal );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawFlushText, "library", "Flushes the text" )
{
    surface()->DrawFlushText();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawGetAlphaMultiplier, "library", "Gets the alpha multiplier" )
{
    lua_pushnumber( L, surface()->DrawGetAlphaMultiplier() );
    return 1;
}
LUA_BINDING_END( "number", "The alpha multiplier" )

LUA_BINDING_BEGIN( Surfaces, DrawGetTextPosition, "library", "Gets the text position" )
{
    int x, y;
    surface()->DrawGetTextPos( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position", "integer", "The y position" )

LUA_BINDING_BEGIN( Surfaces, DrawGetTextureFile, "library", "Gets the texture file" )
{
    char fileName[MAX_PATH];
    int textureId = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" );
    lua_pushboolean( L, surface()->DrawGetTextureFile( textureId, fileName, sizeof( fileName ) ) );
    lua_pushstring( L, fileName );
    return 2;
}
LUA_BINDING_END( "boolean", "True if the texture file was found, false otherwise.", "string", "The texture file" )

LUA_BINDING_BEGIN( Surfaces, DrawGetTextureId, "library", "Gets the texture ID" )
{
    const char *texturePath = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "texturePath" );
    lua_pushinteger( L, surface()->DrawGetTextureId( texturePath ) );
    return 1;
}
LUA_BINDING_END( "integer", "The texture ID" )

LUA_BINDING_BEGIN( Surfaces, DrawGetTextureSize, "library", "Gets the texture size" )
{
    int wide, tall;
    surface()->DrawGetTextureSize( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" ), wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );

    return 2;
}
LUA_BINDING_END( "integer", "The width of the texture", "integer", "The height of the texture" )

LUA_BINDING_BEGIN( Surfaces, DrawLine, "library", "Draws a line" )
{
    float x0 = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x0" );
    float y0 = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y0" );
    float x1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "x1" );
    float y1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "y1" );

    surface()->DrawLine( x0, y0, x1, y1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawOutlinedCircle, "library", "Draws an outlined circle" )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    float radius = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "radius" );
    int segments = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "segments" );

    surface()->DrawOutlinedCircle( x, y, radius, segments );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawOutlinedRectangle, "library", "Draws an outlined rectangle" )
{
    float x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" );
    float y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" );
    float width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "width" );
    float height = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "height" );

    surface()->DrawOutlinedRect( x, y, x + width, y + height );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawPrintText, "library", "Prints text" )
{
    const char *text = LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "text" );
    int bufSize = ( strlen( text ) + 1 ) * sizeof( wchar_t );
    wchar_t *wbuf = static_cast< wchar_t * >( _alloca( bufSize ) );

    if ( wbuf )
    {
        g_pVGuiLocalize->ConvertANSIToUnicode( text, wbuf, bufSize );
        surface()->DrawPrintText(
            wbuf,
            wcslen( wbuf ),
            LUA_BINDING_ARGUMENT_ENUM_WITH_DEFAULT( FontDrawType_t, 3, FONT_DRAW_DEFAULT, "drawType" ) );
    }

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetAlphaMultiplier, "library", "Sets the alpha multiplier" )
{
    surface()->DrawSetAlphaMultiplier( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "alphaMultiplier" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetColor, "library", "Sets the color" )
{
    if ( lua_isnumber( L, 1 ) )
    {
        int r = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "red" );
        int g = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "green" );
        int b = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "blue" );
        int a = LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 255, "alpha" );
        surface()->DrawSetColor( r, g, b, a );
        return 0;
    }

    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    surface()->DrawSetColor( color );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextColor, "library", "Sets the text color" )
{
    if ( lua_isnumber( L, 1 ) )
    {
        surface()->DrawSetTextColor(
            LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "red" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "green" ),
            LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "blue" ),
            LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 4, 255, "alpha" ) );
        return 0;
    }

    lua_Color color = LUA_BINDING_ARGUMENT( luaL_checkcolor, 1, "color" );
    surface()->DrawSetTextColor( color.r(), color.g(), color.b(), color.a() );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextFont, "library", "Sets the text font" )
{
    surface()->DrawSetTextFont( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextPosition, "library", "Sets the text position" )
{
    surface()->DrawSetTextPos(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextScale, "library", "Sets the text scale" )
{
    surface()->DrawSetTextScale(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "xScale" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "yScale" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTexture, "library", "Sets the texture" )
{
    surface()->DrawSetTexture( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextureFile, "library", "Sets the texture file" )
{
    surface()->DrawSetTextureFile(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" ),
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "texturePath" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 3, 0, "hardwareFilter" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 4, true, "shouldForceReload" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextureMaterial, "library", "Sets the texture material" )
{
    g_pMatSystemSurface->DrawSetTextureMaterial(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" ),
        LUA_BINDING_ARGUMENT( luaL_checkmaterial, 2, "material" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawSetTextureRgba, "library", "Sets the texture raw RGBA bytes" )
{
    surface()->DrawSetTextureRGBA(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" ),
        ( const unsigned char * )LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "rgba" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "width" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "height" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 5, 0, "hardwareFilter" ),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 6, true, "shouldForceReload" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawTexturedRectangle, "library", "Draws a textured rectangle" )
{
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x0" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y0" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "x1" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "y1" );

    surface()->DrawTexturedRect( x, y, x + width, y + height );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, DrawTexturedSubRectangle, "library", "Draws a textured sub rectangle" )
{
    int x = LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x0" );
    int y = LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y0" );
    int width = LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "x1" );
    int height = LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "y1" );
    float texs0 = LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "texs0" );
    float text0 = LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "text0" );
    float texs1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "texs1" );
    float text1 = LUA_BINDING_ARGUMENT( luaL_checknumber, 8, "text1" );

    surface()->DrawTexturedSubRect( x, y, x + width, y + height, texs0, text0, texs1, text1 );

    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, EnableMouseCapture, "library", "Enables mouse capture" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->EnableMouseCapture( panel->GetVPanel(), LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "enable" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, FlashWindow, "library", "Flashes the window" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->FlashWindow( panel->GetVPanel(), LUA_BINDING_ARGUMENT( luaL_checkboolean, 2, "flash" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, GetAbsoluteWindowBounds, "library", "Gets the absolute window bounds" )
{
    int x, y, wide, tall;
    surface()->GetAbsoluteWindowBounds( x, y, wide, tall );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 4;
}
LUA_BINDING_END( "integer", "The x position", "integer", "The y position", "integer", "The width", "integer", "The height" )

LUA_BINDING_BEGIN( Surfaces, GetBitmapFontName, "library", "Gets the bitmap font name" )
{
    lua_pushstring( L, surface()->GetBitmapFontName( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fontName" ) ) );
    return 1;
}
LUA_BINDING_END( "string", "The bitmap font name" )

LUA_BINDING_BEGIN( Surfaces, GetCharacterAbcWidth, "library", "Gets the character ABC width" )
{
    int a, b, c;
    surface()->GetCharABCwide(
        LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "character" ),
        a,
        b,
        c );
    lua_pushinteger( L, a );
    lua_pushinteger( L, b );
    lua_pushinteger( L, c );
    return 3;
}
LUA_BINDING_END( "integer", "The A value", "integer", "The B value", "integer", "The C value" )

LUA_BINDING_BEGIN( Surfaces, GetCharacterWidth, "library", "Gets the character width" )
{
    lua_pushinteger( L, surface()->GetCharacterWidth( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "character" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The character width" )

LUA_BINDING_BEGIN( Surfaces, GetEmbeddedPanel, "library", "Gets the embedded panel" )
{
    Panel::PushVPanelLuaInstance( L, surface()->GetEmbeddedPanel() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, GetFontAscent, "library", "Gets the font ascent" )
{
    wchar_t wch[1];
    g_pVGuiLocalize->ConvertANSIToUnicode( LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" ), wch, sizeof( wch ) );
    lua_pushinteger( L, surface()->GetFontAscent( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ), wch[1] ) );
    return 1;
}
LUA_BINDING_END( "integer", "The font ascent" )

LUA_BINDING_BEGIN( Surfaces, GetFontTall, "library", "Gets the font tall" )
{
    lua_pushinteger( L, surface()->GetFontTall( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ) ) );
    return 1;
}
LUA_BINDING_END( "integer", "The font tall" )

LUA_BINDING_BEGIN( Surfaces, GetModalPanel, "library", "Gets the modal panel" )
{
    Panel::PushVPanelLuaInstance( L, surface()->GetModalPanel() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, GetNotifyPanel, "library", "Gets the notify panel" )
{
    Panel::PushVPanelLuaInstance( L, surface()->GetNotifyPanel() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, GetPopup, "library", "Gets the popup" )
{
    Panel::PushVPanelLuaInstance( L, surface()->GetPopup( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "index" ) ) );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, GetPopupCount, "library", "Gets the popup count" )
{
    lua_pushinteger( L, surface()->GetPopupCount() );
    return 1;
}
LUA_BINDING_END( "integer", "The popup count" )

LUA_BINDING_BEGIN( Surfaces, GetProportionalBase, "library", "Gets the proportional base" )
{
    int width, height;
    surface()->GetProportionalBase( width, height );
    lua_pushinteger( L, width );
    lua_pushinteger( L, height );
    return 2;
}
LUA_BINDING_END( "integer", "The width", "integer", "The height" )

LUA_BINDING_BEGIN( Surfaces, GetResolutionKey, "library", "Gets the resolution key" )
{
    lua_pushstring( L, surface()->GetResolutionKey() );
    return 1;
}
LUA_BINDING_END( "string", "The resolution key" )

LUA_BINDING_BEGIN( Surfaces, GetScreenSize, "library", "Gets the screen size" )
{
    int wide, tall;
    surface()->GetScreenSize( wide, tall );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The width", "integer", "The height" )

LUA_BINDING_BEGIN( Surfaces, GetTextSize, "library", "Gets the text size" )
{
    const char *sz = LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "text" );
    int wide = 0;
    int tall = 0;
    int bufSize = ( strlen( sz ) + 1 ) * sizeof( wchar_t );
    wchar_t *wbuf = static_cast< wchar_t * >( _alloca( bufSize ) );
    if ( wbuf )
    {
        surface()->GetTextSize( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ), wbuf, wide, tall );
    }
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 2;
}
LUA_BINDING_END( "integer", "The width", "integer", "The height" )

LUA_BINDING_BEGIN( Surfaces, GetTitle, "library", "Gets the title" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    char szTitle[256];
    const wchar_t *wszTitle = surface()->GetTitle( panel->GetVPanel() );
    g_pVGuiLocalize->ConvertUnicodeToANSI( wszTitle, szTitle, sizeof( szTitle ) );
    lua_pushstring( L, szTitle );
    return 1;
}
LUA_BINDING_END( "string", "The title" )

LUA_BINDING_BEGIN( Surfaces, GetTopmostPopup, "library", "Gets the topmost popup" )
{
    Panel::PushVPanelLuaInstance( L, surface()->GetTopmostPopup() );
    return 1;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, GetWorkspaceBounds, "library", "Gets the workspace bounds" )
{
    int x, y, wide, tall;
    surface()->GetWorkspaceBounds( x, y, wide, tall );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    lua_pushinteger( L, wide );
    lua_pushinteger( L, tall );
    return 4;
}
LUA_BINDING_END( "integer", "The x position", "integer", "The y position", "integer", "The width", "integer", "The height" )

LUA_BINDING_BEGIN( Surfaces, GetZPosition, "library", "Gets the Z position" )
{
    lua_pushnumber( L, surface()->GetZPos() );
    return 1;
}
LUA_BINDING_END( "number", "The Z position" )

LUA_BINDING_BEGIN( Surfaces, HasCursorPositionFunctions, "library", "Checks if the surface has cursor position functions" )
{
    lua_pushboolean( L, surface()->HasCursorPosFunctions() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the surface has cursor position functions, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, HasFocus, "library", "Checks if the surface has focus" )
{
    lua_pushboolean( L, surface()->HasFocus() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the surface has focus, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, Invalidate, "library", "Invalidates the panel" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->Invalidate( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, IsCursorLocked, "library", "Checks if the cursor is locked" )
{
    lua_pushboolean( L, surface()->IsCursorLocked() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the cursor is locked, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, IsCursorVisible, "library", "Checks if the cursor is visible" )
{
    lua_pushboolean( L, surface()->IsCursorVisible() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the cursor is visible, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, IsFontAdditive, "library", "Checks if the font is additive" )
{
    lua_pushboolean( L, surface()->IsFontAdditive( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the font is additive, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, IsMinimized, "library", "Checks if the panel is minimized" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    lua_pushboolean( L, surface()->IsMinimized( panel->GetVPanel() ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the panel is minimized, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, IsTextureIdValid, "library", "Checks if the texture ID is valid" )
{
    lua_pushboolean( L, surface()->IsTextureIDValid( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "textureId" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the texture ID is valid, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, IsWithin, "library", "Checks if the point is within the surface" )
{
    lua_pushboolean( L, surface()->IsWithin( LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the point is within the surface, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, LockCursor, "library", "Locks the cursor" )
{
    surface()->LockCursor();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, MovePopupToBack, "library", "Moves the popup to the back" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->MovePopupToBack( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, MovePopupToFront, "library", "Moves the popup to the front" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->MovePopupToFront( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, NeedKeyboardInput, "library", "Checks if the surface needs keyboard input" )
{
    lua_pushboolean( L, surface()->NeedKBInput() );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the surface needs keyboard input, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, OnScreenSizeChanged, "library", "Called when the screen size changes" )
{
    surface()->OnScreenSizeChanged(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "oldWidth" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "oldHeight" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, PaintTraverse, "library", "Paints the panel" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->PaintTraverse( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, PaintTraverseEx, "library", "Paints the panel" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->PaintTraverseEx(
        panel->GetVPanel(),
        LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optboolean, 2, false, "paintPopups" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, PlaySound, "library", "Plays a sound" )
{
    surface()->PlaySound( LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "sound" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, PopMakeCurrent, "library", "Pops the make current" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->PopMakeCurrent( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, RunFrame, "library", "Runs a frame" )
{
    surface()->RunFrame();
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, SetAllowHtmlJavaScript, "library", "Sets whether HTML JavaScript is allowed" )
{
    surface()->SetAllowHTMLJavaScript( LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "allow" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, SetBitmapFontName, "library", "Sets the bitmap font name" )
{
    surface()->SetBitmapFontName(
        LUA_BINDING_ARGUMENT( luaL_checkstring, 1, "fontName" ),
        LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "fontFilename" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, SetEmbeddedPanel, "library", "Sets the embedded panel" )
{
    Panel *panel = LUA_BINDING_ARGUMENT( luaL_checkpanel, 1, "panel" );
    surface()->SetEmbeddedPanel( panel->GetVPanel() );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, SetFontGlyphSet, "library", "Sets the font glyph set" )
{
    lua_pushboolean( L, surface()->SetFontGlyphSet( LUA_BINDING_ARGUMENT( luaL_checkfont, 1, "font" ), LUA_BINDING_ARGUMENT( luaL_checkstring, 2, "fontName" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "tall" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "weight" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 5, "blur" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 6, "scanlines" ), LUA_BINDING_ARGUMENT( luaL_checknumber, 7, "flags" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 8, 0, "rangeMin" ), LUA_BINDING_ARGUMENT_WITH_DEFAULT( luaL_optnumber, 9, 0, "rangeMax" ) ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the font glyph set was set, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, SetTranslateExtendedKeys, "library", "Sets whether to translate extended keys" )
{
    surface()->SetTranslateExtendedKeys( LUA_BINDING_ARGUMENT( luaL_checkboolean, 1, "state" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, SetWorkspaceInsets, "library", "Sets the workspace insets" )
{
    surface()->SetWorkspaceInsets(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "left" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "top" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 3, "right" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 4, "bottom" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, SupportsFeature, "library", "Checks if the surface supports a feature" )
{
    ISurface::SurfaceFeature_e feature = LUA_BINDING_ARGUMENT_ENUM( ISurface::SurfaceFeature_e, 1, "feature" );
    lua_pushboolean( L, surface()->SupportsFeature( feature ) );
    return 1;
}
LUA_BINDING_END( "boolean", "True if the surface supports the feature, false otherwise." )

LUA_BINDING_BEGIN( Surfaces, SurfaceGetCursorPosition, "library", "Gets the cursor position" )
{
    int x, y;
    surface()->SurfaceGetCursorPos( x, y );
    lua_pushinteger( L, x );
    lua_pushinteger( L, y );
    return 2;
}
LUA_BINDING_END( "integer", "The x position", "integer", "The y position" )

LUA_BINDING_BEGIN( Surfaces, SurfaceSetCursorPosition, "library", "Sets the cursor position" )
{
    surface()->SurfaceSetCursorPos(
        LUA_BINDING_ARGUMENT( luaL_checknumber, 1, "x" ),
        LUA_BINDING_ARGUMENT( luaL_checknumber, 2, "y" ) );
    return 0;
}
LUA_BINDING_END()

LUA_BINDING_BEGIN( Surfaces, UnlockCursor, "library", "Unlocks the cursor" )
{
    surface()->UnlockCursor();
    return 0;
}
LUA_BINDING_END()

/*
** Open surface library
*/
LUALIB_API int luaopen_Surfaces( lua_State *L )
{
    LUA_REGISTRATION_COMMIT_LIBRARY( Surfaces );

    LUA_SET_ENUM_LIB_BEGIN( L, "FONT_DRAW_TYPE" );
    lua_pushenum( L, FontDrawType_t::FONT_DRAW_DEFAULT, "DEFAULT" );
    lua_pushenum( L, FontDrawType_t::FONT_DRAW_NONADDITIVE, "NON_ADDITIVE" );
    lua_pushenum( L, FontDrawType_t::FONT_DRAW_ADDITIVE, "ADDITIVE" );
    lua_pushenum( L, FontDrawType_t::FONT_DRAW_TYPE_COUNT, "TYPE_COUNT" );
    LUA_SET_ENUM_LIB_END( L );

    LUA_SET_ENUM_LIB_BEGIN( L, "SURFACE_FEATURE" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::ANTIALIASED_FONTS, "ANTIALIASED_FONTS" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::DROPSHADOW_FONTS, "DROP_SHADOW_FONTS" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::ESCAPE_KEY, "ESCAPE_KEY" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::OPENING_NEW_HTML_WINDOWS, "OPENING_NEW_HTML_WINDOWS" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::FRAME_MINIMIZE_MAXIMIZE, "FRAME_MINIMIZE_MAXIMIZE" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::OUTLINE_FONTS, "OUTLINE_FONTS" );
    lua_pushenum( L, ISurface::SurfaceFeature_e::DIRECT_HWND_RENDER, "DIRECT_HWND_RENDER" );
    LUA_SET_ENUM_LIB_END( L );

    return 1;
}

/*
** Open FONTFLAG library
*/
LUALIB_API int luaopen_FONTFLAG( lua_State *L )
{
    LUA_SET_ENUM_LIB_BEGIN( L, LUA_FONTFLAGLIBNAME );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_NONE, "NONE" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_ITALIC, "ITALIC" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_UNDERLINE, "UNDERLINE" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_STRIKEOUT, "STRIKEOUT" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_SYMBOL, "SYMBOL" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_ANTIALIAS, "ANTIALIAS" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_GAUSSIANBLUR, "GAUSSIANBLUR" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_ROTARY, "ROTARY" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_DROPSHADOW, "DROPSHADOW" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_ADDITIVE, "ADDITIVE" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_OUTLINE, "OUTLINE" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_CUSTOM, "CUSTOM" );
    lua_pushenum( L, vgui::ISurface::FONTFLAG_BITMAP, "BITMAP" );
    LUA_SET_ENUM_LIB_END( L );
    return 0;
}
