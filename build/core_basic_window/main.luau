function luaugame.setup()
	local screenWidth = 800
	local screenHeight = 450
	InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window")
end

local RAYWHITE = { 245, 245, 245, 255 }
local LIGHTGRAY = { 200, 200, 200, 255 }

function luaugame.draw()
	ClearBackground(RAYWHITE)
	DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY)
end
