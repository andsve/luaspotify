#include "track.h"

/*
 Checklist for sp_track* functions.
 
 [ ] bool 	     sp_track_is_loaded (sp_track *track)
 [ ] sp_error 	 sp_track_error (sp_track *track)
 [ ] bool        sp_track_is_available (sp_session *session, sp_track *track)
 [ ] bool        sp_track_is_local (sp_session *session, sp_track *track)
 [ ] bool        sp_track_is_autolinked (sp_session *session, sp_track *track)
 [ ] bool        sp_track_is_starred (sp_session *session, sp_track *track)
 [ ] void        sp_track_set_starred (sp_session *session, const sp_track **tracks, int num_tracks, bool star)
 [ ] int         sp_track_num_artists (sp_track *track)
 [ ] sp_artist*  sp_track_artist (sp_track *track, int index)
 [ ] sp_album*   sp_track_album (sp_track *track)
 [ ] const char* sp_track_name (sp_track *track)
 [ ] int         sp_track_duration (sp_track *track)
 [ ] int         sp_track_popularity (sp_track *track)
 [ ] int         sp_track_disc (sp_track *track)
 [ ] int         sp_track_index (sp_track *track)
 [ ] sp_track*   sp_localtrack_create (const char *artist, const char *title, const char *album, int length)
 [ ] void        sp_track_add_ref (sp_track *track)
 [ ] void        sp_track_release (sp_track *track)

*/

//const char* sp_track_name (sp_track *track)
int luasp_track_name(lua_State *L) {
  // track_name = spotify.track_name(track)
  
  if (lua_gettop(L) != 1 || !lua_isuserdata(L, 1))
  {
    lua_pushstring(L, "incorrect argument to track_name(...)");
		lua_error(L);
  }
  
  lua_pushstring(L, sp_track_name((sp_track*)lua_touserdata(L, 1)) );
  
  return 1;
}