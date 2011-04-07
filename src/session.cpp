#include "session.h"
#include "playlist.h"
#include <map>
#include <list>
#include <string>

/*
 Checklist for libspotify session functions.

 [x] sp_error sp_session_create (const sp_session_config *config, sp_session **sess)
 [x] void sp_session_release (sp_session *sess)
 [x] void sp_session_login (sp_session *session, const char *username, const char *password)
 [x] sp_user* sp_session_user (sp_session *session)
 [x] void sp_session_logout (sp_session *session)
 [x] sp_connectionstate sp_session_connectionstate (sp_session *session)
 [ ] void* sp_session_userdata (sp_session *session)
 [ ] void sp_session_set_cache_size (sp_session *session, size_t size)
 [x] void sp_session_process_events (sp_session *session, int *next_timeout)
 [ ] sp_error sp_session_player_load (sp_session *session, sp_track *track)
 [ ] void sp_session_player_seek (sp_session *session, int offset)
 [ ] void sp_session_player_play (sp_session *session, bool play)
 [ ] void sp_session_player_unload (sp_session *session)
 [ ] sp_error sp_session_player_prefetch (sp_session *session, sp_track *track)
 [x] sp_playlistcontainer* sp_session_playlistcontainer (sp_session *session)
 [ ] sp_playlist* sp_session_inbox_create (sp_session *session)
 [ ] sp_playlist* sp_session_starred_create (sp_session *session)
 [ ] sp_playlist* sp_session_starred_for_user_create (sp_session *session, const char *canonical_username)
 [ ] sp_playlistcontainer* sp_session_publishedcontainer_for_user_create (sp_session *session, const char *canonical_username)
 [ ] void sp_session_preferred_bitrate (sp_session *session, sp_bitrate bitrate)
 [ ] int sp_session_num_friends (sp_session *session)
 [ ] sp_user* sp_session_friend (sp_session *session, int index)

*/


static sp_session_callbacks session_callbacks;

struct luasp_session_callbacks
{
  lua_State *L;
  
  /* Delayed callback flags */
  bool logged_out_flag;
  bool metadata_updated_flag;
  bool notify_main_thread_flag;
  //bool music_delivery_flag; // should be list instead (not implemented yet)
  bool play_token_lost_flag;
  bool end_of_track_flag;
  bool userinfo_updated_flag;
  bool start_playback_flag;
  bool stop_playback_flag;
  //bool get_audio_buffer_stats_flag; // should be list instead (not implemented yet)
  
  /* Delayed callback buffers/lists. */
  std::list<sp_error> logged_in_buffer;
  std::list<sp_error> connection_error_buffer;
  std::list<std::string> message_to_user_buffer;
  std::list<std::string> log_message_buffer;
  std::list<sp_error> streaming_error_buffer;
  
  /* Lua references */
  int logged_in;
  int logged_out;
  int metadata_updated;
  int connection_error;
  int message_to_user;
  int notify_main_thread;
  int music_delivery;
  int play_token_lost;
  int log_message;
  int end_of_track;
  int streaming_error;
  int userinfo_updated;
  int start_playback;
  int stop_playback;
  int get_audio_buffer_stats;
};

static std::map<sp_session*, luasp_session_callbacks*> active_sessions;

/*
 * Callback collection for libspotify session management
 */
 
void luasp_cb_logged_in(sp_session *session, sp_error error)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->logged_in != 0)
  {
    active_sessions[session]->logged_in_buffer.push_front(error);
  }
}

void luasp_cb_logged_out(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->logged_out != 0)
  {
    active_sessions[session]->logged_out_flag = true;
  }
}

void luasp_cb_metadata_updated(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->metadata_updated != 0)
  {
    active_sessions[session]->metadata_updated_flag = true;
  }
}

void luasp_cb_connection_error(sp_session *session, sp_error error)
{
  printf("connection_error..............\n");
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->connection_error != 0)
  {
    active_sessions[session]->connection_error_buffer.push_front(error);
  }
}

void luasp_cb_message_to_user(sp_session *session, const char *message)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->message_to_user != 0)
  {
    active_sessions[session]->message_to_user_buffer.push_front(std::string(message));
  }
}

void luasp_cb_notify_main_thread(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->notify_main_thread != 0)
  {
    active_sessions[session]->notify_main_thread_flag = true;
  }
}

int  luasp_cb_music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
{
  /* TODO: luasp_cb_music_delivery callback */
  printf("luasp_cb_music_delivery not implemented yet.\n");
  return 0;
}

void luasp_cb_play_token_lost(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->play_token_lost != 0)
  {
    active_sessions[session]->play_token_lost_flag = true;
  }
}

void luasp_cb_log_message(sp_session *session, const char *data)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->log_message != 0)
  {
    active_sessions[session]->log_message_buffer.push_front(std::string(data));
  }
}

void luasp_cb_end_of_track(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->end_of_track != 0)
  {
    active_sessions[session]->end_of_track_flag = true;
  }
}

void luasp_cb_streaming_error(sp_session *session, sp_error error)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->streaming_error != 0)
  {
    active_sessions[session]->streaming_error_buffer.push_front(error);
  }
}

void luasp_cb_userinfo_updated(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->userinfo_updated != 0)
  {
    active_sessions[session]->userinfo_updated_flag = true;
  }
}

void luasp_cb_start_playback(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->start_playback != 0)
  {
    active_sessions[session]->start_playback_flag = true;
  }
}

void luasp_cb_stop_playback(sp_session *session)
{
  if (active_sessions.find(session) != active_sessions.end() && active_sessions[session]->stop_playback != 0)
  {
    active_sessions[session]->stop_playback_flag = true;
  }
}

void luasp_cb_get_audio_buffer_stats(sp_session *session, sp_audio_buffer_stats *stats)
{
  /* TODO: luasp_cb_get_audio_buffer_stats callback */
  printf("luasp_cb_get_audio_buffer_stats not implemented yet.\n");
}

/*
 Checklist for callback implementation.
 
 [x] void(* logged_in )(sp_session *session, sp_error error)
 [x] void(* logged_out )(sp_session *session)
 [x] void(* metadata_updated )(sp_session *session)
 [x] void(* connection_error )(sp_session *session, sp_error error)
 [x] void(* message_to_user )(sp_session *session, const char *message)
 [x] void(* notify_main_thread )(sp_session *session)
 [ ] int (* music_delivery )(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames)
 [x] void(* play_token_lost )(sp_session *session)
 [x] void(* log_message )(sp_session *session, const char *data)
 [x] void(* end_of_track )(sp_session *session)
 [x] void(* streaming_error )(sp_session *session, sp_error error)
 [x] void(* userinfo_updated )(sp_session *session)
 [x] void(* start_playback )(sp_session *session)
 [x] void(* stop_playback )(sp_session *session)
 [ ] void(* get_audio_buffer_stats )(sp_session *session, sp_audio_buffer_stats *stats)
*/



/*
 * Bindings to sp_session_* functions.
 */
 
int luasp_session_create(lua_State *L) {
	// sp_error, sp_session = spotify.session_create(config_table)
	
	if (lua_gettop(L) != 1 || !lua_istable(L, 1))
	{
		lua_pushstring(L, "incorrect argument to session_create(...)");
		lua_error(L);
	}
	
	/*
	 * Construct config structure, fill only with data we can find in the supplied table.
	 */
	sp_session_config spconfig;
	memset(&spconfig, 0, sizeof(spconfig)); /* Zero memory in struct as shown in spshell.c example. */
	
	/* api_version (should be SPOTIFY_API_VERSION) */
	lua_getfield(L, 1, "api_version");
	if (lua_isnumber(L, -1))
	{
		spconfig.api_version = lua_tointeger(L, -1);
	} else {
		spconfig.api_version = SPOTIFY_API_VERSION;
	}
	lua_pop(L, 1);
	
	
	/* const char* cache_location */
	lua_getfield(L, 1, "cache_location");
	if (!lua_isnil(L, -1))
	{
		spconfig.cache_location = lua_tostring(L, -1);
	} else {
		spconfig.cache_location = "tmp";
	}
	lua_pop(L, 1);
	
	
	/* const char* settings_location */
	lua_getfield(L, 1, "settings_location");
	if (!lua_isnil(L, -1))
	{
		spconfig.settings_location = lua_tostring(L, -1);
	} else {
		spconfig.settings_location = "tmp";
	}
	lua_pop(L, 1);
	
	
	/* size_t application_key_size */
	lua_getfield(L, 1, "application_key_size");
	if (lua_isnumber(L, -1))
	{
		spconfig.application_key_size = (size_t)(lua_tointeger(L, -1));
	} else {
		lua_pushstring(L, "incorrect application_key_size sent to session_create(...)");
		lua_error(L);
	}
	lua_pop(L, 1);
	
	
	/* const void* application_key */
	lua_getfield(L, 1, "application_key");
	if (lua_istable(L, -1))
	{
		uint8_t* t_appkey = new uint8_t[spconfig.application_key_size];
		for(size_t i = 0; i < spconfig.application_key_size; ++i)
		{
			lua_rawgeti(L, -1, i+1);
			t_appkey[i] = (uint8_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		
		spconfig.application_key = t_appkey;
	} else {
		lua_pushstring(L, "incorrect application_key sent to session_create(...)");
		lua_error(L);
	}
	lua_pop(L, 1);
	
	
	/* const char* user_agent */
	lua_getfield(L, 1, "user_agent");
	if (!lua_isnil(L, -1))
	{
		spconfig.user_agent = lua_tostring(L, -1);
	} else {
		spconfig.user_agent = "luaspotify";
	}
	lua_pop(L, 1);
  
  
	/* const sp_session_callbacks* callbacks */
	/* We store all lua references/function callbacks that will later be used by libspotifys own callbacks. */
  luasp_session_callbacks *new_luacallbacks = new luasp_session_callbacks;
  new_luacallbacks->L = L;
  lua_getfield(L, 1, "callbacks");
	if (lua_istable(L, -1))
	{
    /*
     * Store Lua callback functions/references that can be called
     * when session_process_events is run.
     */
    
    lua_getfield(L, -1, "logged_in");
    if (lua_isfunction(L, -1))
      new_luacallbacks->logged_in = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
    
    lua_getfield(L, -1, "logged_out");
    if (lua_isfunction(L, -1))
      new_luacallbacks->logged_out = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "metadata_updated");
    if (lua_isfunction(L, -1))
      new_luacallbacks->metadata_updated = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
    
    lua_getfield(L, -1, "connection_error");
    if (lua_isfunction(L, -1))
      new_luacallbacks->connection_error = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
    
    lua_getfield(L, -1, "message_to_user");
    if (lua_isfunction(L, -1))
      new_luacallbacks->message_to_user = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "notify_main_thread");
    if (lua_isfunction(L, -1))
      new_luacallbacks->notify_main_thread = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
    
    /*
    TODO: music_delivery callback/lua ref
    */
    
    lua_getfield(L, -1, "play_token_lost");
    if (lua_isfunction(L, -1))
      new_luacallbacks->play_token_lost = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "log_message");
    if (lua_isfunction(L, -1))
      new_luacallbacks->log_message = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "end_of_track");
    if (lua_isfunction(L, -1))
      new_luacallbacks->end_of_track = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "streaming_error");
    if (lua_isfunction(L, -1))
      new_luacallbacks->streaming_error = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "userinfo_updated");
    if (lua_isfunction(L, -1))
      new_luacallbacks->userinfo_updated = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "start_playback");
    if (lua_isfunction(L, -1))
      new_luacallbacks->start_playback = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
      
    lua_getfield(L, -1, "stop_playback");
    if (lua_isfunction(L, -1))
      new_luacallbacks->stop_playback = luaL_ref(L, LUA_REGISTRYINDEX);
    else
      lua_pop(L, 1);
    
    /*
    TODO: get_audio_buffer_stats callback/lua ref
    */
    
	}
	lua_pop(L, 1);
	
	/* Store callback functions to libspotify config.  */
  session_callbacks.logged_in = &luasp_cb_logged_in;
  session_callbacks.logged_out = &luasp_cb_logged_out;
  session_callbacks.metadata_updated = &luasp_cb_metadata_updated;
  session_callbacks.connection_error = &luasp_cb_connection_error;
  session_callbacks.message_to_user = &luasp_cb_message_to_user;
  session_callbacks.notify_main_thread = &luasp_cb_notify_main_thread;
  session_callbacks.music_delivery = &luasp_cb_music_delivery;
  session_callbacks.play_token_lost = &luasp_cb_play_token_lost;
  session_callbacks.log_message = &luasp_cb_log_message;
  session_callbacks.end_of_track = &luasp_cb_end_of_track;
  session_callbacks.streaming_error = &luasp_cb_streaming_error;
  session_callbacks.userinfo_updated = &luasp_cb_userinfo_updated;
  session_callbacks.start_playback = &luasp_cb_start_playback;
  session_callbacks.stop_playback = &luasp_cb_stop_playback;
  session_callbacks.get_audio_buffer_stats = &luasp_cb_get_audio_buffer_stats;
  spconfig.callbacks = &session_callbacks;
	
	
	/* void* userdata */
	lua_getfield(L, 1, "userdata");
	if (!lua_isnil(L, -1))
	{
		spconfig.userdata = (void*)lua_tostring(L, -1);
	}
	lua_pop(L, 1);
	
	/* bool compress_playlists */
	lua_getfield(L, 1, "compress_playlists");
	if (lua_isboolean(L, -1))
	{
		spconfig.compress_playlists = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);
	
	/* bool dont_save_metadata_for_playlists */
	lua_getfield(L, 1, "dont_save_metadata_for_playlists");
	if (lua_isboolean(L, -1))
	{
		spconfig.dont_save_metadata_for_playlists = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);
	
	/* bool initially_unload_playlists */
	lua_getfield(L, 1, "initially_unload_playlists");
	if (lua_isboolean(L, -1))
	{
		spconfig.initially_unload_playlists = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);
	
	
	
	/*
	 * Create the session, returns the error code and session
	 * (or error string in case of an error) back to lua.
	 */
	sp_error error;
  sp_session* session = NULL;
	
	error = sp_session_create(&spconfig, &session);
	lua_pushinteger(L, (int)error);
	
	if (SP_ERROR_OK != error) {
		lua_pushstring(L, sp_error_message(error) );
	} else {
    lua_pushlightuserdata(L, (void*)session);
    
    /* Update our list of active sessions. */
    active_sessions.insert ( std::pair<sp_session*,luasp_session_callbacks*>(session,new_luacallbacks) );
	}
	
	return 2;
	
}

int luasp_session_release (lua_State *L) {
	/* spotify.session_release(session) */
	
	if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
	{
		lua_pushstring(L, "incorrect argument to session_release(...)");
		lua_error(L);
	}
	
	sp_session *session = (sp_session *)lua_touserdata(L, 1);
  if (session)
	{
		sp_session_release(session);
	}
	
  return 0;
}

int luasp_session_login (lua_State *L) {
  // spotify.session_login(session, username, password)
  
  if (lua_gettop(L) != 3 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to session_login(...)");
		lua_error(L);
  }
  
  sp_session_login ((sp_session*)lua_touserdata(L, 1), lua_tostring(L, 2), lua_tostring(L, 3));
  
  return 0;
}

int luasp_session_logout (lua_State *L) {
  // spotify.session_logout(session)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to session_logout(...)");
		lua_error(L);
  }
  
  sp_session_logout((sp_session*)lua_touserdata(L, 1));
  
  return 0;
}

int luasp_session_user (lua_State *L) {
  // user = spotify.session_user(session)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to session_user(...)");
		lua_error(L);
  }
  
  lua_pushlightuserdata(L, (void*)sp_session_user((sp_session*)lua_touserdata(L, 1)));
  
  return 1;
}

int luasp_session_connectionstate (lua_State *L) {
  // state = spotify.session_connectionstate(session)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to session_connectionstate(...)");
		lua_error(L);
  }
  
  lua_pushinteger(L, sp_session_connectionstate((sp_session*)lua_touserdata(L, 1)));
  
  return 1;
}

int luasp_session_process_events (lua_State *L) {
  // timeout = spotify.session_process_events(session)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to session_process_events(...)");
		lua_error(L);
  }
  
  int next_timeout;
  sp_session* session = (sp_session*)lua_touserdata(L, 1);
  sp_session_process_events(session, &next_timeout);
  
  
  /* Pump internal callbacks (ie. Lua-ref callbacks) */
  _pump_session_callbacks(session, L);
  _pump_playlist_callbacks(session, L);
  
  
  /* Return next timeout form sp_session_process_events. */
  lua_pushinteger(L, next_timeout);
  
  return 1;
}


int luasp_session_playlistcontainer(lua_State *L) {
  // playlistcontainer = spotify.session_playlistcontainer(session)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to session_playlistcontainer(...)");
		lua_error(L);
  }
  
  lua_pushlightuserdata(L, (void*)sp_session_playlistcontainer((sp_session*)lua_touserdata(L, 1)) );
  
  return 1;
}


/*
 * Internal session functions.
 */
 
void _pump_session_callbacks(sp_session* session, lua_State *L) {
  /*
   * Call Lua callbacks.
   */
   
  /* logged_in */
  while (!active_sessions[session]->logged_in_buffer.empty())
  {
    /* Call Lua function for logged_in, for each element in the list/buffer. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->logged_in);
    lua_pushlightuserdata(L, (void*)session);
    lua_pushinteger(L, active_sessions[session]->logged_in_buffer.back());
    active_sessions[session]->logged_in_buffer.pop_back();
    lua_pcall(L, 2, 0, 0);
  }
  
  /* logged_out */
  if (active_sessions[session]->logged_out_flag)
  {
    /* Call Lua function for logged_out. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->logged_out);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->logged_out_flag = false;
  }
  
  /* metadata_updated */
  if (active_sessions[session]->metadata_updated_flag)
  {
    /* Call Lua function for metadata_updated. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->metadata_updated);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->metadata_updated_flag = false;
  }
  
  /* connection_error */
  while (!active_sessions[session]->connection_error_buffer.empty())
  {
    /* Call Lua function for connection_error, for each element in the list/buffer. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->connection_error);
    lua_pushlightuserdata(L, (void*)session);
    lua_pushinteger(L, active_sessions[session]->connection_error_buffer.back());
    active_sessions[session]->connection_error_buffer.pop_back();
    lua_pcall(L, 2, 0, 0);
  }
  
  /* message_to_user */
  while (!active_sessions[session]->message_to_user_buffer.empty())
  {
    /* Call Lua function for message_to_user, for each element in the list/buffer. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->message_to_user);
    lua_pushlightuserdata(L, (void*)session);
    lua_pushstring(L, (active_sessions[session]->message_to_user_buffer.back()).c_str());
    active_sessions[session]->message_to_user_buffer.pop_back();
    lua_pcall(L, 2, 0, 0);
  }
  
  /* notify_main_thread */
  if (active_sessions[session]->notify_main_thread_flag)
  {
    /* Call Lua function for metadata_updated. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->notify_main_thread);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->notify_main_thread_flag = false;
  }
  
  /* play_token_lost */
  if (active_sessions[session]->play_token_lost_flag)
  {
    /* Call Lua function for play_token_lost. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->play_token_lost);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->play_token_lost_flag = false;
  }
  
  /* log_message */
  while (!active_sessions[session]->log_message_buffer.empty())
  {
      /* Call Lua function for log_message, for each element in the list/buffer. */
      lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->log_message);
      lua_pushlightuserdata(L, (void*)session);
      lua_pushlstring(L, (active_sessions[session]->log_message_buffer.back()).c_str(), (active_sessions[session]->log_message_buffer.back()).size()-1);
      active_sessions[session]->log_message_buffer.pop_back();
      lua_pcall(L, 2, 0, 0);
  }
  
  /* end_of_track */
  if (active_sessions[session]->end_of_track_flag)
  {
    /* Call Lua function for end_of_track. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->end_of_track);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->end_of_track_flag = false;
  }
  
  /* streaming_error */
  while (!active_sessions[session]->streaming_error_buffer.empty())
  {
      /* Call Lua function for streaming_error, for each element in the list/buffer. */
      lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->streaming_error);
      lua_pushlightuserdata(L, (void*)session);
      lua_pushinteger(L, active_sessions[session]->streaming_error_buffer.back());
      active_sessions[session]->streaming_error_buffer.pop_back();
      lua_pcall(L, 2, 0, 0);
  }
  
  /* userinfo_updated */
  if (active_sessions[session]->userinfo_updated_flag)
  {
    /* Call Lua function for userinfo_updated. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->userinfo_updated);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->userinfo_updated_flag = false;
  }
  
  /* start_playback */
  if (active_sessions[session]->start_playback_flag)
  {
    /* Call Lua function for start_playback. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->start_playback);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->start_playback_flag = false;
  }
  
  /* stop_playback */
  if (active_sessions[session]->stop_playback_flag)
  {
    /* Call Lua function for stop_playback. */
    lua_rawgeti(L, LUA_REGISTRYINDEX, active_sessions[session]->stop_playback);
    lua_pushlightuserdata(L, (void*)session);
    lua_pcall(L, 1, 0, 0);
    
    active_sessions[session]->stop_playback_flag = false;
  }
}
