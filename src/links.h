#ifndef _LUASPOTIFY_LINKS_H_
#define _LUASPOTIFY_LINKS_H_

#include "spotify.h"

int luasp_link_create_from_string(lua_State *L);

int luasp_link_type(lua_State *L);

int luasp_link_as_track(lua_State *L);

#endif /* _LUASPOTIFY_LINKS_H_ */