#include <thread>

#include "GifPlayerLua.h"
#include <sol/sol.hpp>
#include <GifPlayer.h>
#include "DrawHook.h"

std::vector<class GifPlayerLua *> g_players;
std::mutex						  g_payer_mutex;

class GifPlayerLua {
private:
	std::shared_ptr<GifPlayer> gif_player_ptr;
	bool					   is_initialized = false;

public:
	GifPlayerLua( const char *name ) {
		std::thread( [this, name]() {
			gif_player_ptr = std::make_shared<GifPlayer>( name );
			{
				std::lock_guard<std::mutex> lock( g_payer_mutex );
				g_players.push_back( this );
			}
		} ).detach();
	}

	~GifPlayerLua() {
		std::lock_guard<std::mutex> lock( g_payer_mutex );
		auto						it = std::find( g_players.begin(), g_players.end(), this );
		if ( it != g_players.end() ) g_players.erase( it );
	}

	size_t ProcessPlay() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return 0;
		return (size_t)gif_player_ptr->ProcessPlay();
	}

	void ResetLoop() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return;
		gif_player_ptr->ResetLoop();
	}

	bool IsLoopEnded() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return false;
		return gif_player_ptr->IsLoopEnded();
	}

	bool IsDrawBg() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return false;
		return gif_player_ptr->IsDrawBg();
	}

	void ToggleBg( bool state ) {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return;
		gif_player_ptr->ToggleBg( state );
	}

	void ToggleBgNoUpdate( bool state ) {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return;
		gif_player_ptr->ToggleBgNoUpdate( state );
	}

	void Update() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return;
		gif_player_ptr->Update();
	}

	int GetWidth() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return 0;
		return gif_player_ptr->GetWidth();
	}

	int GetHeight() {
		if ( gif_player_ptr.get() == nullptr || !is_initialized ) return 0;
		return gif_player_ptr->GetHeight();
	}

	void Invalidate() {
		if ( gif_player_ptr.get() == nullptr ) return;
		gif_player_ptr->Invalidate();
	}

	bool IsLoaded() { return gif_player_ptr.get() != nullptr; }

	bool IsInitialized() { return is_initialized; }

	void Initialize() {
		if ( gif_player_ptr.get() == nullptr || is_initialized ) return;
		gif_player_ptr->Initialize( DrawHook::d3d9_device() );
		is_initialized = true;
	}
};

sol::table open( sol::this_state ts ) {
	sol::state_view lua( ts );
	sol::table		module = lua.create_table();
	module["VERSION"]	   = "9e4db64498289920f23d54ffbb15e6d771446f77";

	module.new_usertype<GifPlayerLua>( "GifPlayer", sol::constructors<GifPlayerLua( const char * )>(), "ProcessPlay",
									   &GifPlayerLua::ProcessPlay, "ResetLoop", &GifPlayerLua::ResetLoop, "IsLoopEnded",
									   &GifPlayerLua::IsLoopEnded, "IsDrawBg", &GifPlayerLua::IsDrawBg, "ToggleBg", &GifPlayerLua::ToggleBg,
									   "ToggleBgNoUpdate", &GifPlayerLua::ToggleBgNoUpdate, "Update", &GifPlayerLua::Update, "GetWidth",
									   &GifPlayerLua::GetWidth, "GetHeight", &GifPlayerLua::GetHeight, "IsLoaded", &GifPlayerLua::IsLoaded,
									   "IsInitialized", &GifPlayerLua::IsInitialized, "Initialize", &GifPlayerLua::Initialize );

	static bool hookInitialized = false;
	if ( !hookInitialized ) {
		hookInitialized = true;
		static DrawHook draw;
		draw.onPreReset += [] {
			std::lock_guard<std::mutex> lock( g_payer_mutex );
			for ( auto &&player : g_players ) player->Invalidate();
		};
		draw.onPostReset += [] {
			std::lock_guard<std::mutex> lock( g_payer_mutex );
			for ( auto &&player : g_players ) player->Initialize();
		};
	}

	return module;
}

extern "C" GIFPLAYERLUA_EXPORT int luaopen_GifPlayerLua( lua_State *L ) {
	return (sol::c_call<decltype( &( open ) ), &( open )>)( L );
}
