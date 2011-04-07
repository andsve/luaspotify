#ifndef _LUASPOTIFY_SESSION_H_
#define _LUASPOTIFY_SESSION_H_

#include <spotify.h>

int luasp_session_release(lua_State *L);
int luasp_session_create(lua_State *L);
int luasp_session_login(lua_State *L);
int luasp_session_user(lua_State *L);
int luasp_session_logout(lua_State *L);

int luasp_session_connectionstate(lua_State *L);

int luasp_session_process_events(lua_State *L);
int luasp_session_playlistcontainer(lua_State *L);

/* Internal session functions. */
void _pump_session_callbacks(sp_session* session, lua_State *L);

#endif /* _LUASPOTIFY_SESSION_H_ */