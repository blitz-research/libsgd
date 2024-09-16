-- Run 'helloworld.lua' with LuaJIT
--
-- cp ../lib/sgd_dynamic.dll .
-- luac helloworld.lua

local bit = require("bit")
local sgd = require("sgd")

sgd.init()

sgd.createWindow(640, 480, "Hello LibSGD in Lua!", 0)
sgd.setClearColor(1, 0.5, 0, 1)

while (bit.band(sgd.pollEvents(), sgd.EVENT_MASK_CLOSE_CLICKED) == 0) do
    sgd.renderScene()
    sgd.present()
end

sgd.terminate()
