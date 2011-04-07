// luaspotify
#include "spotify.h"
#include "session.h"
#include "user.h"
#include "playlist.h"
#include "track.h"
#include "links.h"

extern "C" {
	int LUA_API luaopen_spotify (lua_State *L);
}

static int luasp_debug (lua_State *L) {
	//printf("SP_TOPLIST_TYPE_TRACKS: %d\n", SP_TOPLIST_TYPE_TRACKS);
	return 0;
}

int luasp_error_message(lua_State *L) {
  // error_string = spotify.error_message(error)
  
  if (lua_gettop(L) != 1 || !lua_isnumber(L, 1))
	{
		lua_pushstring(L, "incorrect argument to error_message(...)");
		lua_error(L);
	}
	
  lua_pushstring(L, sp_error_message((sp_error)lua_tointeger(L, 1)) );
  
  return 1;
}

int LUA_API luaopen_spotify (lua_State *L) {
	struct luaL_reg driver[] = {
	  
	  /* genaral functions*/
		{"debug", luasp_debug},
		{"error_message", luasp_error_message},
		
		/* sp_session_* (from session.h) */
		{"session_create", luasp_session_create},
		{"session_release", luasp_session_release},
		{"session_login", luasp_session_login},
		{"session_user", luasp_session_user},
    {"session_logout", luasp_session_logout},
		
		{"session_connectionstate", luasp_session_connectionstate},
		
		{"session_process_events", luasp_session_process_events},
		
		{"session_playlistcontainer", luasp_session_playlistcontainer},
		
		/* sp_user_* (from user.h) */
		{"user_canonical_name", luasp_user_canonical_name},
		{"user_display_name", luasp_user_display_name},
		{"user_is_loaded", luasp_user_is_loaded},
		{"user_full_name", luasp_user_full_name},
		{"user_picture", luasp_user_picture},
		{"user_relation_type", luasp_user_relation_type},
		{"user_add_ref", luasp_user_add_ref},
		{"user_release", luasp_user_release},
		
		/* sp_playlist* (from playlist.h) */
		{"playlist_is_loaded", luasp_playlist_is_loaded},
		{"playlist_add_callbacks", luasp_playlist_add_callbacks},
		
		{"playlist_num_tracks", luasp_playlist_num_tracks},
		{"playlist_track", luasp_playlist_track},
		
		{"playlist_name", luasp_playlist_name},
		
    {"playlist_add_tracks", luasp_playlist_add_tracks},
		
		{"playlist_create", luasp_playlist_create},
		
		{"playlist_release", luasp_playlist_release},
		
		{"playlistcontainer_add_callbacks", luasp_playlistcontainer_add_callbacks},
		
	  {"playlistcontainer_num_playlists", luasp_playlistcontainer_num_playlists},
	  {"playlistcontainer_playlist", luasp_playlistcontainer_playlist},
	  {"playlistcontainer_playlist_type", luasp_playlistcontainer_playlist_type},
	  
	  {"playlistcontainer_release", luasp_playlistcontainer_release},
	  
	  /* sp_track_* (from track.h) */
	  {"track_name", luasp_track_name},
	  
	  /* sp_link_* (from links.h) */
	  {"link_create_from_string", luasp_link_create_from_string},
	  {"link_type", luasp_link_type},
	  
	  {"link_as_track", luasp_link_as_track},
		
		{NULL, NULL},
	};
	
	luaL_openlib (L, LUASPOTIFY_TABLENAME, driver, 0);
	
	
	/*
	 *  Add enums for each subsystem.
	 */
	
	/*
	 * Error enums.
	 */
  lua_newtable(L);
  LUA_ENUM(L, SP_ERROR_OK);
  LUA_ENUM(L, SP_ERROR_BAD_API_VERSION);
  LUA_ENUM(L, SP_ERROR_API_INITIALIZATION_FAILED);
  LUA_ENUM(L, SP_ERROR_TRACK_NOT_PLAYABLE);
  LUA_ENUM(L, SP_ERROR_RESOURCE_NOT_LOADED);
  LUA_ENUM(L, SP_ERROR_BAD_APPLICATION_KEY);
  LUA_ENUM(L, SP_ERROR_BAD_USERNAME_OR_PASSWORD);
  LUA_ENUM(L, SP_ERROR_USER_BANNED);
  LUA_ENUM(L, SP_ERROR_UNABLE_TO_CONTACT_SERVER);
  LUA_ENUM(L, SP_ERROR_CLIENT_TOO_OLD);
  LUA_ENUM(L, SP_ERROR_OTHER_PERMANENT);
  LUA_ENUM(L, SP_ERROR_BAD_USER_AGENT);
  LUA_ENUM(L, SP_ERROR_MISSING_CALLBACK);
  LUA_ENUM(L, SP_ERROR_INVALID_INDATA);
  LUA_ENUM(L, SP_ERROR_INDEX_OUT_OF_RANGE);
  LUA_ENUM(L, SP_ERROR_USER_NEEDS_PREMIUM);
  LUA_ENUM(L, SP_ERROR_OTHER_TRANSIENT);
  LUA_ENUM(L, SP_ERROR_IS_LOADING);
  LUA_ENUM(L, SP_ERROR_NO_STREAM_AVAILABLE);
  LUA_ENUM(L, SP_ERROR_PERMISSION_DENIED);
  LUA_ENUM(L, SP_ERROR_INBOX_IS_FULL);
  LUA_ENUM(L, SP_ERROR_NO_CACHE);
  LUA_ENUM(L, SP_ERROR_NO_SUCH_USER);
  lua_setfield (L, -2, "error");
	
	/*
	 * Session subsystem enums.
	 */
	
	/* sp_connectionstate enums */
	lua_newtable(L);
  LUA_ENUM(L, SP_CONNECTION_STATE_LOGGED_OUT);
  LUA_ENUM(L, SP_CONNECTION_STATE_LOGGED_IN);
  LUA_ENUM(L, SP_CONNECTION_STATE_DISCONNECTED);
  LUA_ENUM(L, SP_CONNECTION_STATE_UNDEFINED);
	lua_setfield (L, -2, "connectionstate");
	
	/* sp_connectionstate enums */
	lua_newtable(L);
  LUA_ENUM(L, SP_SAMPLETYPE_INT16_NATIVE_ENDIAN);
	lua_setfield (L, -2, "sampletype");
	
	/*
	TODO: sp_bitrate enums (?)
  */
  
  /* sp_playlist_type enums */
	lua_newtable(L);
  LUA_ENUM(L, SP_PLAYLIST_TYPE_PLAYLIST);
  LUA_ENUM(L, SP_PLAYLIST_TYPE_START_FOLDER);
  LUA_ENUM(L, SP_PLAYLIST_TYPE_END_FOLDER);
  LUA_ENUM(L, SP_PLAYLIST_TYPE_PLACEHOLDER);
	lua_setfield (L, -2, "playlist_type");
	
	
	/*
	 * User subsystem enums.
	 */
	
	/* sp_relation_type enums */
	lua_newtable(L);
  LUA_ENUM(L, SP_RELATION_TYPE_UNKNOWN);
  LUA_ENUM(L, SP_RELATION_TYPE_NONE);
  LUA_ENUM(L, SP_RELATION_TYPE_UNIDIRECTIONAL);
  LUA_ENUM(L, SP_RELATION_TYPE_BIDIRECTIONAL);
	lua_setfield (L, -2, "relation_type");
	
	/*
	 * Links subsystem enums.
	 */
	 
	/* sp_linktype enums */
  lua_newtable(L);
  LUA_ENUM(L, SP_LINKTYPE_INVALID);
  LUA_ENUM(L, SP_LINKTYPE_TRACK);
  LUA_ENUM(L, SP_LINKTYPE_ALBUM);
  LUA_ENUM(L, SP_LINKTYPE_ARTIST);
  LUA_ENUM(L, SP_LINKTYPE_SEARCH);
  LUA_ENUM(L, SP_LINKTYPE_PLAYLIST);
  LUA_ENUM(L, SP_LINKTYPE_PROFILE);
  LUA_ENUM(L, SP_LINKTYPE_STARRED);
  LUA_ENUM(L, SP_LINKTYPE_LOCALTRACK);
  lua_setfield (L, -2, "linktype");

	
	return 1;
}
