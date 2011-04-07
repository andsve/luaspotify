#include "user.h"

/*
 [x] const char * sp_user_canonical_name (sp_user *user)
 [x] const char * sp_user_display_name (sp_user *user)
 [x] bool sp_user_is_loaded (sp_user *user)
 [x] const char * sp_user_full_name (sp_user *user)
 [x] const char * sp_user_picture (sp_user *user)
 [x] sp_relation_type sp_user_relation_type (sp_session *session, sp_user *user)
 [x] void sp_user_add_ref (sp_user *user)
 [x] void sp_user_release (sp_user *user)
*/


int luasp_user_canonical_name(lua_State *L) {
  // canonical_name = spotify.user_canonical_name(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_canonical_name(...)");
		lua_error(L);
  }
  
  lua_pushstring(L, sp_user_canonical_name((sp_user*)lua_touserdata(L, 1)) );
    
  return 1;
}

int luasp_user_display_name(lua_State *L) {
  // display_name = spotify.user_display_name(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_display_name(...)");
		lua_error(L);
  }
  
  lua_pushstring(L, sp_user_display_name((sp_user*)lua_touserdata(L, 1)) );
  
  return 1;
}

int luasp_user_is_loaded(lua_State *L) {
  // bool = spotify.user_is_loaded(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_is_loaded(...)");
		lua_error(L);
  }
  
  lua_pushboolean(L, sp_user_is_loaded((sp_user*)lua_touserdata(L, 1)) );
  
  return 1;
}

int luasp_user_full_name(lua_State *L) {
  // full_name = spotify.user_full_name(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_full_name(...)");
		lua_error(L);
  }
  
  const char* full_name = sp_user_full_name((sp_user*)lua_touserdata(L, 1));
  if (full_name == NULL)
  {
    lua_pushnil(L);
  } else {
    lua_pushstring(L, full_name );
  }
  
  return 1;
}

int luasp_user_picture(lua_State *L) {
  // image_url = spotify.user_picture(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_picture(...)");
		lua_error(L);
  }
  
  const char* img_url = sp_user_picture((sp_user*)lua_touserdata(L, 1));
  if (img_url == NULL)
  {
    lua_pushnil(L);
  } else {
    lua_pushstring(L, img_url );
  }
  
  return 1;
}

int luasp_user_relation_type(lua_State *L) {
  // relation_type = spotify.user_relation_type(session, user)
  
  if (lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_isuserdata(L, 2))
  {
    lua_pushstring(L, "incorrect argument to user_relation_type(...)");
		lua_error(L);
  }
  
  lua_pushinteger(L, sp_user_relation_type((sp_session*)lua_touserdata(L, 1), (sp_user*)lua_touserdata(L, 2)) );
  
  return 1;
}

int luasp_user_add_ref(lua_State *L) {
  // spotify.user_add_ref(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_add_ref(...)");
		lua_error(L);
  }
  
  sp_user_add_ref((sp_user*)lua_touserdata(L, 1));
  
  return 0;
}

int luasp_user_release(lua_State *L) {
  // spotify.user_release(user)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to user_release(...)");
		lua_error(L);
  }
  
  sp_user_release((sp_user*)lua_touserdata(L, 1));
  
  return 0;
}
