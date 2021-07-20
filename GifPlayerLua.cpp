#include "GifPlayerLua.h"
#include <sol/sol.hpp>
#include <GifPlayer.h>
#include "DrawHook.h"

std::vector<class GifPlayerLua *> g_players;
class GifPlayerLua : public GifPlayer {
public:
	GifPlayerLua( const char *name ) : GifPlayer( name ) {
		g_players.push_back( this );
		Initialize( DrawHook::d3d9_device() );
	}
	~GifPlayerLua() {
		auto it = std::find( g_players.begin(), g_players.end(), this );
		if ( it != g_players.end() ) g_players.erase( it );
	}
	size_t ProcessPlay() { return (size_t)GifPlayer::ProcessPlay(); }
};

sol::table open( sol::this_state ts ) {
	sol::state_view lua( ts );
	sol::table		module = lua.create_table();
	module["VERSION"]	   = "9e4db64498289920f23d54ffbb15e6d771446f77";

	module.new_usertype<GifPlayerLua>( "GifPlayer", sol::constructors<GifPlayerLua( const char * )>(),
									   "ProcessPlay", &GifPlayerLua::ProcessPlay,
									   "ResetLoop", &GifPlayerLua::ResetLoop,
									   "IsLoopEnded", &GifPlayerLua::IsLoopEnded,
									   "IsDrawBg", &GifPlayerLua::IsDrawBg,
									   "ToggleBg", &GifPlayerLua::ToggleBg,
									   "ToggleBgNoUpdate", &GifPlayerLua::ToggleBgNoUpdate,
									   "Update", &GifPlayerLua::Update,
									   "GetWidth", &GifPlayerLua::GetWidth,
									   "GetHeight", &GifPlayerLua::GetHeight);

	static bool hookInitialized = false;
	if ( !hookInitialized ) {
		hookInitialized = true;
		static DrawHook draw;
		draw.onPreReset += [] {
			for ( auto &&player : g_players ) player->Invalidate();
		};
		draw.onPostReset += [] {
			for ( auto &&player : g_players ) player->Initialize( DrawHook::d3d9_device() );
		};
	}

	return module;
}

extern "C" GIFPLAYERLUA_EXPORT int luaopen_GifPlayerLua( lua_State *L ) {
	return (sol::c_call<decltype( &( open ) ), &( open )>)( L );
}
