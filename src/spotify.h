#ifndef _LUASPOTIFY_SPOTIFY_H_
#define _LUASPOTIFY_SPOTIFY_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// lua
extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}

// libspotify
#include <libspotify/api.h>

#ifdef WIN32
  #define LUA_API __declspec(dllexport)
#endif

#define LUASPOTIFY_TABLENAME "spotify"

#define LUA_ENUM(L, name) \
  lua_pushlstring(L, #name, sizeof(#name)-1); \
  lua_pushnumber(L, name); \
  lua_settable(L, -3);


#endif /* _LUASPOTIFY_SPOTIFY_H_ */