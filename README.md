## Lua usage

```lua
-- Load library
local gif = require 'GifPlayerLua'

-- Show version of GifPlayer
print(gif.VERSION)

-- Load gif image
local player = gif.GifPlayer.new("anim.gif")

-- Play anim
local dx9tex = player:ProcessPlay()

-- Restart animation
if player:IsLoopEnded() then
    player:ResetLoop()
end

-- Force enable background
if not player:IsDrawBg then
    player:ToggleBg(true)
end

-- Disable background on event onResetDevice
player:ToggleBgNoUpdate(false)

-- Force update textures
player:Update()

-- Get image size
local width = player:GetWidth()
local height = player:GetHeight()
```

 
