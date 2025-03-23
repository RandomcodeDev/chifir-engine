-- Build configuration template
-- Copyright 2025 Randomcode Developers

-- The fields are pretty self explanatory, they generally correspond to a macro with the same name
-- Changing them will force a total rebuild, so avoid it.

-- The game_*name fields will be changed to just be defaults once an equivalent to Source's gameinfo.txt has been made
-- Basically like how Source's launcher is named hl2.exe and it tries to find the hl2 folder, but -game changes it

-- This is used for the save folder on PC, and as the name of the LauncherMain executable
game_name = "FalseKing"
-- Used as the default window title, not sure if it actually shows up anywhere else yet
game_display_name = "False King"

-- Mainly used for shortening __FILE__ in log messages
repo_name = "chifir-engine"

-- Also has to be changed in private/public/Application.nmeta, and probably somewhere else (you'll probably notice)
-- TODO: move this somewhere private? since it's a placeholder that's used by secret code, it probably should be
switch_title_id = "0100694203488000"
