#ifndef _LUASPOTIFY_PLAYLIST_H_
#define _LUASPOTIFY_PLAYLIST_H_

#include <spotify.h>

int luasp_playlist_is_loaded(lua_State *L);
int luasp_playlist_add_callbacks(lua_State *L);

int luasp_playlist_num_tracks(lua_State *L);
int luasp_playlist_track(lua_State *L);

int luasp_playlist_name(lua_State *L);

int luasp_playlist_add_tracks(lua_State *L);

int luasp_playlist_create(lua_State *L);

int luasp_playlist_release(lua_State *L);

int luasp_playlistcontainer_add_callbacks(lua_State *L);

int luasp_playlistcontainer_num_playlists(lua_State *L);
int luasp_playlistcontainer_playlist(lua_State *L);
int luasp_playlistcontainer_playlist_type(lua_State *L);

int luasp_playlistcontainer_release(lua_State *L);

void _pump_playlist_callbacks(sp_session* session, lua_State *L);


#endif /* _LUASPOTIFY_PLAYLIST_H_ */