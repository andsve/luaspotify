#include "links.h"

/*

 Checklist for sp_link_* functions.

 [x] sp_link*    sp_link_create_from_string (const char *link)
 [ ] sp_link*    sp_link_create_from_track (sp_track *track, int offset)
 [ ] sp_link*    sp_link_create_from_album (sp_album *album)
 [ ] sp_link*    sp_link_create_from_artist (sp_artist *artist)
 [ ] sp_link*    sp_link_create_from_search (sp_search *search)
 [ ] sp_link*    sp_link_create_from_playlist (sp_playlist *playlist)
 [ ] sp_link*    sp_link_create_from_user (sp_user *user)
 [ ] int         sp_link_as_string (sp_link *link, char *buffer, int buffer_size)
 [x] sp_linktype sp_link_type (sp_link *link)
 [x] sp_track*   sp_link_as_track (sp_link *link)
 [ ] sp_track*   sp_link_as_track_and_offset (sp_link *link, int *offset)
 [ ] sp_album*   sp_link_as_album (sp_link *link)
 [ ] sp_artist*  sp_link_as_artist (sp_link *link)
 [ ] sp_user*    sp_link_as_user (sp_link *link)
 [ ] void        sp_link_add_ref (sp_link *link)
 [ ] void        sp_link_release (sp_link *link)

*/


int luasp_link_create_from_string(lua_State *L) {
  // link = spotify.link_create_from_string(uri)
  
  if (lua_gettop(L) != 1 || !lua_isstring(L, 1))
  {
    lua_pushstring(L, "incorrect argument to link_create_from_string(...)");
		lua_error(L);
  }
  
  sp_link* t_link = sp_link_create_from_string(lua_tostring(L, 1));
  
  if (t_link)
    lua_pushlightuserdata(L,  t_link);
  else
    lua_pushnil(L);
  
  return 1;
}

int luasp_link_type(lua_State *L) {
  // link_type = spotify.link_type(link)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to link_create_from_string(...)");
		lua_error(L);
  }
  
  lua_pushinteger(L, sp_link_type((sp_link*)lua_touserdata(L, 1)) );
  
  return 1;
}

//sp_track*   sp_link_as_track (sp_link *link)
int luasp_link_as_track(lua_State *L) {
  // track = spotify.link_as_track(link)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to link_as_track(...)");
		lua_error(L);
  }
  
  lua_pushlightuserdata(L, sp_link_as_track((sp_link*)lua_touserdata(L, 1)) );
  
  return 1;
}

