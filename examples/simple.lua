---------------------------------------------------------------------
--
-- luaspotify example
-- 
-- This example logs into the Spotify service, list all the playlists
-- of the user, and finally logout.
--
---------------------------------------------------------------------


require("spotify")
--require("appkey")

-- keep this secret!
local app_key = {
	--0x00, ..., 0x00,
}

--
-- Spotify settings
--
local username = "username"
local password = "password"

--
-- Globals
--
local running = true
local has_printed_list = false
local has_logged_in = false
local playlistcontainer = nil
local session = nil

--
-- Callbacks
-- Remember: These will and can only be called after you have
--           called spotify.session_process_events(session)!
--           This because the real callbacks are buffered internally
--           of the luaspotify module.
--

-- playlistcontainer callbacks
function container_loaded( ps, userdata )
  print("[container_loaded]")
  
  -- list all playlists in the container
  local num_playlists = spotify.playlistcontainer_num_playlists(ps)
  local padding = ""
  for i=0,num_playlists-1 do
    
    local playlist = spotify.playlistcontainer_playlist(ps, i)
    
    if (spotify.playlistcontainer_playlist_type(ps, i) == spotify.playlist_type.SP_PLAYLIST_TYPE_START_FOLDER) then
      padding = "     "
      print(padding .. "- Folder -----------------")
    elseif (spotify.playlistcontainer_playlist_type(ps, i) == spotify.playlist_type.SP_PLAYLIST_TYPE_END_FOLDER) then
      print(padding .. "--------------------------")
      padding = ""
    else
      print(padding .. " ( " .. tostring(spotify.playlist_num_tracks(playlist)) .. " ) " .. tostring(spotify.playlist_name(playlist)))
    end
    
  end
  
  has_printed_list = true
  
  -- we are done, logout!
  spotify.session_logout(session)
end

-- session callbacks
function notify_main_thread( s )
  -- no real need to take care of at the moment
  -- since we allready are calling session_process_events
end
function log_message( s, msg ) print("[log_message] msg: " .. tostring(msg) ) end
function logged_in( s, error ) 
  print("[logged_in] result: " .. tostring(error) .. ", " .. tostring(spotify.error_message(error)) )
  if (error == spotify.error.SP_ERROR_OK) then
    has_logged_in = true
    
    -- get playlistcontainer for user
    playlistcontainer = spotify.session_playlistcontainer(s)
    spotify.playlistcontainer_add_callbacks(playlistcontainer, {container_loaded = container_loaded})
  end
end
function logged_out( s )
  print("[logged_out]")
  if (has_printed_list) then
    running = false
  end
end
function message_to_user( s, msg ) print("[message_to_user] msg: " .. tostring(msg)) end
function userinfo_updated( s ) print("[userinfo_updated] " .. tostring(s)) end
function connection_error( s, error ) print("[connection_error] " .. tostring(error) .. ", " .. tostring(spotify.error_message(error)) ) end

--
-- libspotify configuration
--
local config = {application_key = app_key,
                application_key_size = #app_key,
                user_agent = "luaspotify-example-simple",
                callbacks = {notify_main_thread = notify_main_thread,
                             message_to_user = message_to_user,
                             userinfo_updated = userinfo_updated,
                             connection_error = connection_error,
                             logged_in = logged_in,
                             logged_out = logged_out,
                             log_message = log_message}
               }


--
-- Lets do some magic!
--

-- create/init session
err, session = spotify.session_create(config)
if (err ~= spotify.error.SP_ERROR_OK) then
  print("session creation failed, err: " .. tostring(err) .. ", '" .. tostring(spotify.error_message(err)) .. "'")
else
  print("session creation succeeded")
  
  -- login (will hopefully eventually call the logged_in callback)
  spotify.session_login(session, username, password)
  
  -- our main loop-de-loop
  while running do
    spotify.session_process_events(session)
  end
  
  print("releasing session")
  
  -- clean up
  spotify.session_release(session)
end
