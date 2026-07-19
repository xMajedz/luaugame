w, h = 1600, 900
xpos, ypos = w/2, h/2
xvel, yvel = 200, 200
gravity = 1E3
radius = 100

InitWindow(1600, 900, "tsoding ball")

function draw()
        local dt = GetFrameTime()

	yvel = yvel + gravity * dt

	local x = xpos + xvel * dt
	if x + radius > w or x - radius < 0 then
		xvel = xvel * -1.00
	else
		xpos = x
	end

	local y = ypos + yvel * dt
	if y + radius > h or y - radius < 0 then
		yvel = yvel * -1.00
	else
		ypos = y
	end
  
	ClearBackground({9, 9, 9, 255})
	DrawCircle(xpos, ypos, radius, {230, 41, 55, 255});
end
