require("../spotify")

-- keep this secret!
local app_key = {
	0x00, ...	0x00,
}

--
-- Spotify settings
--
local username = ""
local password = ""

--
-- Callbacks
-- Remember: These will and can only be called after you have
--           called spotify.session_process_events(session)!
--

-- session callbacks
function notify_main_thread( s )
  -- no really need to take care of at the moment
  -- since we allready are calling session_process_events
end

function log_message( s, msg ) print("[log_message] msg: " .. tostring(msg)) end
function logged_in( s, error ) print("[logged_in] result: " .. tostring(error)) end
function logged_out( s ) print("[logged_out]") end
function message_to_user( s, msg ) print("[message_to_user] msg: " .. tostring(msg)) end
function userinfo_updated( s ) print("[userinfo_updated] " .. tostring(s)) end
function connection_error( s, error ) print("[connection_error] " .. tostring(error)) end

-- playlist callbacks
function playlist_state_changed( pl, userdata ) print("[playlist_state_changed]") end

-- playlistcontainer callbacks
function container_loaded( ps, userdata )
  print("[container_loaded]")
  
end

--
-- libspotify configuration
--
local config = {application_key = app_key,
                application_key_size = #app_key,
                user_agent = "luaspotify-sample",
                callbacks = {notify_main_thread = notify_main_thread,
                             message_to_user = message_to_user,
                             userinfo_updated = userinfo_updated,
                             connection_error = connection_error,
                             logged_in = logged_in,
                             logged_out = logged_out,
                             log_message = log_message}
               }
               
--
-- Globals
--
local running = true


--
-- Lets do some magic!
--

-- create/init session
local err, session = spotify.session_create(config)
