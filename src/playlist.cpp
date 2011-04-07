#include "playlist.h"
#include <map>
#include <list>
#include <string>

/*

 Checklist for sp_playlistcontainer callbacks.

 [x] void(* playlist_added )(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata)
 [x] void(* playlist_removed )(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata)
 [x] void(* playlist_moved )(sp_playlistcontainer *pc, sp_playlist *playlist, int position, int new_position, void *userdata)
 [x] void(* container_loaded )(sp_playlistcontainer *pc, void *userdata)

*/

struct luasp_playlistcontainer_callbacks
{
 lua_State *L;
 
 struct luasp_playlistcontainer_playlist_added_t
 {
   sp_playlist *playlist;
   int position;
   void *userdata;
 };
 
 struct luasp_playlistcontainer_playlist_removed_t
 {
   sp_playlist *playlist;
   int position;
   void *userdata;
 };
 
 struct luasp_playlistcontainer_playlist_moved_t
 {
   sp_playlist *playlist;
   int position;
   int new_position;
   void *userdata;
 };
 
 struct luasp_playlistcontainer_container_loaded_t
 {
   void *userdata;
 };

 /* Delayed callback buffers/lists. */
 std::list<luasp_playlistcontainer_playlist_added_t> playlist_added_buffer;
 std::list<luasp_playlistcontainer_playlist_removed_t> playlist_removed_buffer;
 std::list<luasp_playlistcontainer_playlist_moved_t> playlist_moved_buffer;
 std::list<luasp_playlistcontainer_container_loaded_t> container_loaded_buffer;

 /* Lua references */
 int playlist_added;
 int playlist_removed;
 int playlist_moved;
 int container_loaded;
 
};

static std::map<sp_playlistcontainer*, luasp_playlistcontainer_callbacks> active_playlistcontainer;


void luasp_cb_playlist_added(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata) {
  if (active_playlistcontainer.find(pc) != active_playlistcontainer.end() &&
      active_playlistcontainer[pc].playlist_added != 0)
  {
    luasp_playlistcontainer_callbacks::luasp_playlistcontainer_playlist_added_t t_pcontainer;
    t_pcontainer.playlist = playlist;
    t_pcontainer.position = position;
    t_pcontainer.userdata = userdata;
    active_playlistcontainer[pc].playlist_added_buffer.push_front(t_pcontainer);
  }
}

void luasp_cb_playlist_removed(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata) {
  if (active_playlistcontainer.find(pc) != active_playlistcontainer.end() &&
      active_playlistcontainer[pc].playlist_removed != 0)
  {
    luasp_playlistcontainer_callbacks::luasp_playlistcontainer_playlist_removed_t t_pcontainer;
    t_pcontainer.playlist = playlist;
    t_pcontainer.position = position;
    t_pcontainer.userdata = userdata;
    active_playlistcontainer[pc].playlist_removed_buffer.push_front(t_pcontainer);
  }
}

void luasp_cb_playlist_moved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, int new_position, void *userdata) {
  if (active_playlistcontainer.find(pc) != active_playlistcontainer.end() &&
      active_playlistcontainer[pc].playlist_moved != 0)
  {
    luasp_playlistcontainer_callbacks::luasp_playlistcontainer_playlist_moved_t t_pcontainer;
    t_pcontainer.playlist = playlist;
    t_pcontainer.position = position;
    t_pcontainer.new_position = new_position;
    t_pcontainer.userdata = userdata;
    active_playlistcontainer[pc].playlist_moved_buffer.push_front(t_pcontainer);
  }
}

void luasp_cb_container_loaded(sp_playlistcontainer *pc, void *userdata) {
  if (active_playlistcontainer.find(pc) != active_playlistcontainer.end() &&
      active_playlistcontainer[pc].container_loaded != 0)
  {
    luasp_playlistcontainer_callbacks::luasp_playlistcontainer_container_loaded_t t_pcontainer;
    t_pcontainer.userdata = userdata;
    active_playlistcontainer[pc].container_loaded_buffer.push_front(t_pcontainer);
  }
}

sp_playlistcontainer_callbacks g_playlistcontainer_callbacks;



/*

 Checklist for sp_playlist callbacks.

 [ ] void(* tracks_added )(sp_playlist *pl, sp_track *const *tracks, int num_tracks, int position, void *userdata)
 [ ] void(* tracks_removed )(sp_playlist *pl, const int *tracks, int num_tracks, void *userdata)
 [ ] void(* tracks_moved )(sp_playlist *pl, const int *tracks, int num_tracks, int new_position, void *userdata)
 [ ] void(* playlist_renamed )(sp_playlist *pl, void *userdata)
 [x] void(* playlist_state_changed )(sp_playlist *pl, void *userdata)
 [ ] void(* playlist_update_in_progress )(sp_playlist *pl, bool done, void *userdata)
 [ ] void(* playlist_metadata_updated )(sp_playlist *pl, void *userdata)
 [ ] void(* track_created_changed )(sp_playlist *pl, int position, sp_user *user, int when, void *userdata)
 [ ] void(* track_seen_changed )(sp_playlist *pl, int position, bool seen, void *userdata)
 [ ] void(* description_changed )(sp_playlist *pl, const char *desc, void *userdata)
 [ ] void(* image_changed )(sp_playlist *pl, const byte *image, void *userdata)
 [ ] void(* track_message_changed )(sp_playlist *pl, int position, const char *message, void *userdata)
 [ ] void(* subscribers_changed )(sp_playlist *pl, void *userdata)

*/

struct luasp_playlist_callbacks
{
 lua_State *L;

 /* Delayed callback buffers/lists. */
 std::list<void *> playlist_state_changed_buffer;

 /* Lua references */
 int playlist_state_changed;
 
};

static std::map<sp_playlist*, luasp_playlist_callbacks> active_playlist;

void luasp_cb_playlist_state_changed(sp_playlist *pl, void *userdata) {
  if (active_playlist.find(pl) != active_playlist.end() &&
      active_playlist[pl].playlist_state_changed != 0)
  {
    active_playlist[pl].playlist_state_changed_buffer.push_front(userdata);
  }
}

sp_playlist_callbacks g_playlist_callbacks;



/*
 * sp_playlist* functions.
 */

int luasp_playlist_is_loaded(lua_State *L) {
  // bool = spotify.playlist_is_loaded(playlist)
  
  if (!lua_gettop(L) == 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to playlist_is_loaded(...)");
		lua_error(L);
  }
  
  lua_pushboolean(L, sp_playlist_is_loaded((sp_playlist*)lua_touserdata(L, 1)) );
  
  return 1;
}

int luasp_playlist_add_callbacks(lua_State *L) {
  // spotify.playlist_add_callbacks(playlist, callback_table, [userdata])
  
  if (!(lua_gettop(L) == 2 || lua_gettop(L) == 3) || !lua_isuserdata(L, 1) || !lua_istable(L, 2))
  {
    lua_pushstring(L, "incorrect argument to playlist_add_callbacks(...)");
		lua_error(L);
  }
  
  /* Build sp_playlist_callbacks struct from table data. */
  g_playlist_callbacks.playlist_state_changed = &luasp_cb_playlist_state_changed;
  
  /* Add Lua refs to callbacks. */
  luasp_playlist_callbacks new_luacallbacks;
  new_luacallbacks.L = L;
  
  /* playlist_state_changed */
  lua_getfield(L, 2, "playlist_state_changed");
  if (lua_isfunction(L, -1))
    new_luacallbacks.playlist_state_changed = luaL_ref(L, LUA_REGISTRYINDEX);
  else
    lua_pop(L, 1);
    
  /* Update our list of active playlists map. */
  active_playlist.insert ( std::pair<sp_playlist*, luasp_playlist_callbacks>((sp_playlist*)lua_touserdata(L, 1),new_luacallbacks) );
  
  /* Tell libspotify about callbacks. */
  sp_playlist_add_callbacks((sp_playlist*)lua_touserdata(L, 1), &g_playlist_callbacks, (void*)lua_tostring(L, 3));
  
  
  return 0;
}

int luasp_playlist_num_tracks(lua_State *L) {
  // number = spotify.playlist_num_tracks(playlist)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to playlist_num_tracks(...)");
		lua_error(L);
  }
  
  lua_pushinteger(L, sp_playlist_num_tracks((sp_playlist*)lua_touserdata(L, 1)) );
  
  return 1;
}

int luasp_playlist_track(lua_State *L) {
  // track = spotify.playlist_track(playlist, index)
  
  if (lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_isnumber(L, 2))
  {
    lua_pushstring(L, "incorrect argument to playlist_track(...)");
		lua_error(L);
  }
  
  lua_pushlightuserdata(L, sp_playlist_track((sp_playlist*)lua_touserdata(L, 1), lua_tointeger(L, 2)) );
  
  return 1;
}


int luasp_playlist_name(lua_State *L) {
  // name = spotify.playlist_name(playlist)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to playlist_name(...)");
		lua_error(L);
  }
  
  lua_pushstring(L, sp_playlist_name((sp_playlist*)lua_touserdata(L, 1)) );
  
  return 1;
}

// sp_error 	sp_playlist_add_tracks (sp_playlist *playlist, const sp_track **tracks, int num_tracks, int position, sp_session *session)
int luasp_playlist_add_tracks(lua_State *L) {
  // error = spotify.playlist_add_tracks(playlist, table_with_tracks, number_of_tracks, position, session)
  
  if (lua_gettop(L) != 5
      || !lua_isuserdata(L, 1)
      || !lua_istable(L, 2)
      || !lua_isnumber(L, 3)
      || !lua_isnumber(L, 4)
      || !lua_isuserdata(L, 5))
  {
    lua_pushstring(L, "incorrect argument to playlist_add_tracks(...)");
		lua_error(L);
  }
  
  int num_tracks = lua_tointeger(L, 3);
  const sp_track **tracks = new const sp_track*[num_tracks];
  
  for(int i = 0; i < num_tracks; ++i)
  {
    lua_rawgeti(L, 2, i+1);
    tracks[i] = (sp_track*)lua_touserdata(L, -1);
    lua_pop(L, 1);
  }
  
  lua_pushinteger(L, sp_playlist_add_tracks((sp_playlist*)lua_touserdata(L, 1), tracks, num_tracks, lua_tointeger(L, 4), (sp_session*)lua_touserdata(L, 5)) );
  
  delete [] tracks;
  
  return 1;
}


int luasp_playlist_create(lua_State *L) {
  // playlist = spotify.playlist_create(session, link)
  
  if (lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_isuserdata(L, 2))
  {
    lua_pushstring(L, "incorrect argument to playlist_create(...)");
		lua_error(L);
  }
  
  sp_playlist* t_playlist = sp_playlist_create((sp_session*)lua_touserdata(L, 1), (sp_link*)lua_touserdata(L, 2));
  lua_pushlightuserdata(L, t_playlist );
  
  return 1;
}

//void 	sp_playlist_release (sp_playlist *playlist)
int luasp_playlist_release(lua_State *L) {
  // spotify.playlist_release(playlist)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to playlist_release(...)");
		lua_error(L);
  }
  
  sp_playlist_release( (sp_playlist*)lua_touserdata(L, 1) );
  
  return 0;
}


int luasp_playlistcontainer_num_playlists(lua_State *L) {
  // num_playlists = spotify.playlistcontainer_num_playlists(playlistcontainer)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to playlistcontainer_num_playlists(...)");
		lua_error(L);
  }
  
  lua_pushinteger(L, sp_playlistcontainer_num_playlists((sp_playlistcontainer*)lua_touserdata(L, 1)) );
  
  return 1;
}

int luasp_playlistcontainer_add_callbacks(lua_State *L) {
  // spotify.playlistcontainer_add_callbacks(playlistcontainer, table_with_callbacks, [userdata])
  
  if (!(lua_gettop(L) == 2 || lua_gettop(L) == 3) || !lua_isuserdata(L, 1) || !lua_istable(L, 2))
  {
    lua_pushstring(L, "incorrect argument to playlistcontainer_add_callbacks(...)");
		lua_error(L);
  }
  
  /* Build sp_playlistcontainer_callbacks struct from table data. */
  g_playlistcontainer_callbacks.playlist_added = &luasp_cb_playlist_added;
  g_playlistcontainer_callbacks.playlist_removed = &luasp_cb_playlist_removed;
  g_playlistcontainer_callbacks.playlist_moved = &luasp_cb_playlist_moved;
  g_playlistcontainer_callbacks.container_loaded = &luasp_cb_container_loaded;
  
  /* Add Lua refs to callbacks. */
  luasp_playlistcontainer_callbacks new_luacallbacks;// = new luasp_playlistcontainer_callbacks;
  new_luacallbacks.L = L;
  
  /* playlist_added */
  lua_getfield(L, 2, "playlist_added");
  if (lua_isfunction(L, -1))
    new_luacallbacks.playlist_added = luaL_ref(L, LUA_REGISTRYINDEX);
  else
    lua_pop(L, 1);
    
  /* playlist_removed */
  lua_getfield(L, 2, "playlist_removed");
  if (lua_isfunction(L, -1))
    new_luacallbacks.playlist_removed = luaL_ref(L, LUA_REGISTRYINDEX);
  else
    lua_pop(L, 1);
    
  /* playlist_moved */
  lua_getfield(L, 2, "playlist_moved");
  if (lua_isfunction(L, -1))
    new_luacallbacks.playlist_moved = luaL_ref(L, LUA_REGISTRYINDEX);
  else
    lua_pop(L, 1);
  
  /* compress_playlists */
  lua_getfield(L, 2, "container_loaded");
  if (lua_isfunction(L, -1))
    new_luacallbacks.container_loaded = luaL_ref(L, LUA_REGISTRYINDEX);
  else
    lua_pop(L, 1);
    
  /* Update our list of active playlistcontainers. */
  active_playlistcontainer.insert ( std::pair<sp_playlistcontainer*, luasp_playlistcontainer_callbacks>((sp_playlistcontainer*)lua_touserdata(L, 1),new_luacallbacks) );
  
  /* Tell libspotify about callbacks. */
  sp_playlistcontainer_add_callbacks((sp_playlistcontainer*)lua_touserdata(L, 1), &g_playlistcontainer_callbacks, (void*)lua_tostring(L, 3));
  
  return 0;
}


int luasp_playlistcontainer_playlist(lua_State *L) {
  // playlist = spotify.playlistcontainer_playlist(playlistcontainer, index)
  
  if (lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_isnumber(L, 2))
  {
    lua_pushstring(L, "incorrect argument to playlistcontainer_playlist(...)");
		lua_error(L);
  }
  
  lua_pushlightuserdata(L, sp_playlistcontainer_playlist((sp_playlistcontainer*)lua_touserdata(L, 1), lua_tointeger(L, 2)) );
  
  return 1;
}

int luasp_playlistcontainer_playlist_type(lua_State *L) {
  // playlist_type = spotify.playlistcontainer_playlist_type(playlistcontainer, index)
  
  if (lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_isnumber(L, 2))
  {
    lua_pushstring(L, "incorrect argument to playlistcontainer_playlist_type(...)");
		lua_error(L);
  }
  
  lua_pushinteger(L, sp_playlistcontainer_playlist_type((sp_playlistcontainer*)lua_touserdata(L, 1), lua_tointeger(L, 2)) );
  
  return 1;
}


int luasp_playlistcontainer_release(lua_State *L) {
  // spotify.playlistcontainer_release(playlistcontainer)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to playlistcontainer_release(...)");
		lua_error(L);
  }
  
  sp_playlistcontainer_release((sp_playlistcontainer*)lua_touserdata(L, 1));
  
  return 0;
}

/*

 Checklist for playlist subsystem.

 [x] bool 	sp_playlist_is_loaded (sp_playlist *playlist)
 [x] void 	sp_playlist_add_callbacks (sp_playlist *playlist, sp_playlist_callbacks *callbacks, void *userdata)
 [ ] void 	sp_playlist_remove_callbacks (sp_playlist *playlist, sp_playlist_callbacks *callbacks, void *userdata)
 [x] int 	sp_playlist_num_tracks (sp_playlist *playlist)
 [x] sp_track * 	sp_playlist_track (sp_playlist *playlist, int index)
 [ ] int 	sp_playlist_track_create_time (sp_playlist *playlist, int index)
 [ ] sp_user * 	sp_playlist_track_creator (sp_playlist *playlist, int index)
 [ ] bool 	sp_playlist_track_seen (sp_playlist *playlist, int index)
 [ ] sp_error 	sp_playlist_track_set_seen (sp_playlist *playlist, int index, bool seen)
 [ ] const char * 	sp_playlist_track_message (sp_playlist *playlist, int index)
 [x] const char * 	sp_playlist_name (sp_playlist *playlist)
 [ ] sp_error 	sp_playlist_rename (sp_playlist *playlist, const char *new_name)
 [ ] sp_user * 	sp_playlist_owner (sp_playlist *playlist)
 [ ] bool 	sp_playlist_is_collaborative (sp_playlist *playlist)
 [ ] void 	sp_playlist_set_collaborative (sp_playlist *playlist, bool collaborative)
 [ ] void 	sp_playlist_set_autolink_tracks (sp_playlist *playlist, bool link)
 [ ] const char * 	sp_playlist_get_description (sp_playlist *playlist)
 [ ] bool 	sp_playlist_get_image (sp_playlist *playlist, byte image[20])
 [ ] bool 	sp_playlist_has_pending_changes (sp_playlist *playlist)
 [x] sp_error 	sp_playlist_add_tracks (sp_playlist *playlist, const sp_track **tracks, int num_tracks, int position, sp_session *session)
 [ ] sp_error 	sp_playlist_remove_tracks (sp_playlist *playlist, const int *tracks, int num_tracks)
 [ ] sp_error 	sp_playlist_reorder_tracks (sp_playlist *playlist, const int *tracks, int num_tracks, int new_position)
 [ ] unsigned int 	sp_playlist_num_subscribers (sp_playlist *playlist)
 [ ] sp_subscribers * 	sp_playlist_subscribers (sp_playlist *playlist)
 [ ] void 	sp_playlist_subscribers_free (sp_subscribers *subscribers)
 [ ] void 	sp_playlist_update_subscribers (sp_session *session, sp_playlist *playlist)
 [ ] bool 	sp_playlist_is_in_ram (sp_session *session, sp_playlist *playlist)
 [ ] void 	sp_playlist_set_in_ram (sp_session *session, sp_playlist *playlist, bool in_ram)
 [x] sp_playlist * 	sp_playlist_create (sp_session *session, sp_link *link)
 [ ] void 	sp_playlist_add_ref (sp_playlist *playlist)
 [x] void 	sp_playlist_release (sp_playlist *playlist)
 [x] void 	sp_playlistcontainer_add_callbacks (sp_playlistcontainer *pc, sp_playlistcontainer_callbacks *callbacks, void *userdata)
 [ ] void 	sp_playlistcontainer_remove_callbacks (sp_playlistcontainer *pc, sp_playlistcontainer_callbacks *callbacks, void *userdata)
 [x] int 	sp_playlistcontainer_num_playlists (sp_playlistcontainer *pc)
 [x] sp_playlist * 	sp_playlistcontainer_playlist (sp_playlistcontainer *pc, int index)
 [x] sp_playlist_type 	sp_playlistcontainer_playlist_type (sp_playlistcontainer *pc, int index)
 [ ] sp_error 	sp_playlistcontainer_playlist_folder_name (sp_playlistcontainer *pc, int index, char *buffer, int buffer_size)
 [ ] sp_uint64 	sp_playlistcontainer_playlist_folder_id (sp_playlistcontainer *pc, int index)
 [ ] sp_playlist * 	sp_playlistcontainer_add_new_playlist (sp_playlistcontainer *pc, const char *name)
 [ ] sp_playlist * 	sp_playlistcontainer_add_playlist (sp_playlistcontainer *pc, sp_link *link)
 [ ] sp_error 	sp_playlistcontainer_remove_playlist (sp_playlistcontainer *pc, int index)
 [ ] sp_error 	sp_playlistcontainer_move_playlist (sp_playlistcontainer *pc, int index, int new_position, bool dry_run)
 [ ] sp_error 	sp_playlistcontainer_add_folder (sp_playlistcontainer *pc, int index, const char *name)
 [ ] sp_user * 	sp_playlistcontainer_owner (sp_playlistcontainer *pc)
 [ ] void 	sp_playlistcontainer_add_ref (sp_playlistcontainer *pc)
 [x] void 	sp_playlistcontainer_release (sp_playlistcontainer *pc)

*/

/*
 * Internal luaspotify functions.
 */

void _pump_playlist_callbacks(sp_session* session, lua_State *L) {
  
  /*
   * Loop and call playlistcontainer callbacks.
   */
  for(std::map<sp_playlistcontainer*, luasp_playlistcontainer_callbacks>::iterator ps_it = active_playlistcontainer.begin();
      ps_it != active_playlistcontainer.end();
      ++ps_it)
  {
    
    /* playlist_added */
    while (!(*ps_it).second.playlist_added_buffer.empty())
    {
        /* Call Lua function for playlist_added, for each element in the list/buffer. */
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*ps_it).second.playlist_added);
        lua_pushlightuserdata(L, (void*)(*ps_it).first);
        lua_pushlightuserdata(L, (void*)(*ps_it).second.playlist_added_buffer.back().playlist);
        lua_pushinteger(L, (*ps_it).second.playlist_added_buffer.back().position);
        lua_pushstring(L, (char*)(*ps_it).second.playlist_added_buffer.back().userdata);
        (*ps_it).second.playlist_added_buffer.pop_back();
        lua_pcall(L, 4, 0, 0);
    }
    
    /* playlist_removed */
    while (!(*ps_it).second.playlist_removed_buffer.empty())
    {
        /* Call Lua function for playlist_removed, for each element in the list/buffer. */
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*ps_it).second.playlist_removed);
        lua_pushlightuserdata(L, (void*)(*ps_it).first);
        lua_pushlightuserdata(L, (void*)(*ps_it).second.playlist_removed_buffer.back().playlist);
        lua_pushinteger(L, (*ps_it).second.playlist_removed_buffer.back().position);
        lua_pushstring(L, (char*)(*ps_it).second.playlist_removed_buffer.back().userdata);
        (*ps_it).second.playlist_removed_buffer.pop_back();
        lua_pcall(L, 4, 0, 0);
    }
    
    /* playlist_moved */
    while (!(*ps_it).second.playlist_moved_buffer.empty())
    {
        /* Call Lua function for playlist_moved, for each element in the list/buffer. */
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*ps_it).second.playlist_moved);
        lua_pushlightuserdata(L, (void*)(*ps_it).first);
        lua_pushlightuserdata(L, (void*)(*ps_it).second.playlist_moved_buffer.back().playlist);
        lua_pushinteger(L, (*ps_it).second.playlist_moved_buffer.back().position);
        lua_pushinteger(L, (*ps_it).second.playlist_moved_buffer.back().new_position);
        lua_pushstring(L, (char*)(*ps_it).second.playlist_moved_buffer.back().userdata);
        (*ps_it).second.playlist_moved_buffer.pop_back();
        lua_pcall(L, 5, 0, 0);
    }
        
    /* container_loaded */
    while (!(*ps_it).second.container_loaded_buffer.empty())
    {
        /* Call Lua function for container_loaded, for each element in the list/buffer. */
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*ps_it).second.container_loaded);
        lua_pushlightuserdata(L, (void*)(*ps_it).first);
        lua_pushstring(L, (char*)(*ps_it).second.container_loaded_buffer.back().userdata);
        (*ps_it).second.container_loaded_buffer.pop_back();
        lua_pcall(L, 2, 0, 0);
    }
  }
  
  /*
   * Loop and call playlist callbacks.
   */
  for(std::map<sp_playlist*, luasp_playlist_callbacks>::iterator pl_it = active_playlist.begin();
      pl_it != active_playlist.end();
      ++pl_it)
  {
    
    /*  */
    while (!(*pl_it).second.playlist_state_changed_buffer.empty())
    {
        /* Call Lua function for playlist_state_changed, for each element in the list/buffer. */
        lua_rawgeti(L, LUA_REGISTRYINDEX, (*pl_it).second.playlist_state_changed);
        lua_pushlightuserdata(L, (void*)(*pl_it).first);
        lua_pushstring(L, (char*)(*pl_it).second.playlist_state_changed_buffer.back());
        (*pl_it).second.playlist_state_changed_buffer.pop_back();
        lua_pcall(L, 2, 0, 0);
    }
    
  }
  
}

