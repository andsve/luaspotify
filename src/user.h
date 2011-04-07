#ifndef _LUASPOTIFY_USER_H_
#define _LUASPOTIFY_USER_H_

#include <spotify.h>

int luasp_user_canonical_name(lua_State *L);
int luasp_user_display_name(lua_State *L);
int luasp_user_is_loaded(lua_State *L);
int luasp_user_full_name(lua_State *L);
int luasp_user_picture(lua_State *L);
int luasp_user_relation_type(lua_State *L);
int luasp_user_add_ref(lua_State *L);
int luasp_user_release(lua_State *L);

#endif /* _LUASPOTIFY_USER_H_ */