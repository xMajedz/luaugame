#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>

#include <raylib.h>

struct Bytecode {
	size_t size;
	char* data;
};

struct State {
	bool UpdateFuncExists = true;
	bool DrawFuncExists = true;
	bool CustomInitWindow;
} luaugame_State;

int luaugame_dostring(lua_State* L, const char* string, const char* chunkname)
{
	Bytecode bytecode = {0};
	bytecode.data = luau_compile(string, strlen(string), NULL, &bytecode.size);
	int result = luau_load(L, chunkname, bytecode.data, bytecode.size, 0);
	free(bytecode.data);
	return lua_pcall(L, 0, 0, 0);
}

int luaugame_dostring(lua_State* L, const char* string)
{
	return luaugame_dostring(L, string, "=dostring");
}

int luaugame_dofile(lua_State* L, const char* file_name)
{
	char* source = LoadFileText(file_name);
	int status = luaugame_dostring(L, source, "=dofile");
	UnloadFileText(source);
	return status;
}

int luaugame_setup(lua_State* L)
{
	lua_pushstring(L, "setup");
	lua_gettable(L, -2);
	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
		} else {
		}
	} else {
		lua_pop(L, -2);
	}

	if (!luaugame_State.CustomInitWindow)
		InitWindow(800, 450, "untitled luaugame");

	return 1;
}

int luaugame_update(lua_State* L)
{
	lua_pushstring(L, "update");
	lua_gettable(L, -2);
	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
		} else {
		}
	} else {
		luaugame_State.UpdateFuncExists = false;
		lua_pop(L, -2);
	}

	return 1;
}

int luaugame_draw(lua_State* L)
{
	lua_pushstring(L, "draw");
	lua_gettable(L, -2);
	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
		} else {
		}
	} else {
		luaugame_State.DrawFuncExists = false;
		lua_pop(L, -2);
	}

	return 1;
}

int luaugame_InitWindow(lua_State* L)
{
	const char* title = lua_tostring(L, -1);
	int h = lua_tonumber(L, -2);
	int w = lua_tonumber(L, -3);
	InitWindow(w, h, title);

	luaugame_State.CustomInitWindow = true;

	return 1;
}

int luaugame_ClearBackground(lua_State* L)
{
	if (!lua_istable(L, -1))
		return 1;

	unsigned char color[4];

	lua_rawgeti(L, -1, 1);
	color[0] = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	color[1] = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	color[2] = lua_tonumber(L, -1);
	lua_rawgeti(L, -4, 4);
	color[3] = lua_tonumber(L, -1);

	ClearBackground((Color){color[0], color[1], color[2], color[3]});

	return 1;
}

int luaugame_DrawText(lua_State* L)
{
	unsigned char color[4];

	lua_rawgeti(L, -1, 1);
	color[0] = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	color[1] = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	color[2] = lua_tonumber(L, -1);
	lua_rawgeti(L, -4, 4);
	color[3] = lua_tonumber(L, -1);

	lua_Number fontSize = lua_tonumber(L, -6);
	lua_Number y = lua_tonumber(L, -7);
	lua_Number x = lua_tonumber(L, -8);
	const char* text = lua_tostring(L, -9);

	DrawText(text, x, y, fontSize, (Color){color[0], color[1], color[2], color[3]});

	return 1;
}

int luaugame_DrawCircle(lua_State* L)
{
	unsigned char color[4];

	lua_rawgeti(L, -1, 1);
	color[0] = lua_tonumber(L, -1);
	lua_rawgeti(L, -2, 2);
	color[1] = lua_tonumber(L, -1);
	lua_rawgeti(L, -3, 3);
	color[2] = lua_tonumber(L, -1);
	lua_rawgeti(L, -4, 4);
	color[3] = lua_tonumber(L, -1);

	lua_Number radius = lua_tonumber(L, -6);
	lua_Number posY = lua_tonumber(L, -7);
	lua_Number posX = lua_tonumber(L, -8);

	DrawCircle(posX, posY, radius, (Color){color[0], color[1], color[2], color[3]});
	return 1;
}

int luaugame_GetFrameTime(lua_State* L)
{
	lua_pushnumber(L, GetFrameTime());
	return 1;
}

int luaugame_GetScreenSize(lua_State* L)
{
	lua_pushnumber(L, GetScreenWidth());
	printf("%d", GetScreenWidth());
	lua_pushnumber(L, GetScreenHeight());
	printf("%d", GetScreenHeight());
	return 2;
}

int main(int argc, char** argv)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushcfunction(L, luaugame_InitWindow, "InitWindow");
	lua_setglobal(L, "InitWindow");

	lua_pushcfunction(L, luaugame_ClearBackground, "ClearBackground");
	lua_setglobal(L, "ClearBackground");

	lua_pushcfunction(L, luaugame_DrawText, "DrawText");
	lua_setglobal(L, "DrawText");

	lua_pushcfunction(L, luaugame_DrawCircle, "DrawCircle");
	lua_setglobal(L, "DrawCircle");

	lua_pushcfunction(L, luaugame_GetScreenSize, "GetScreenSize");
	lua_setglobal(L, "GetScreenSize");

	lua_pushcfunction(L, luaugame_GetFrameTime, "GetFrameTime");
	lua_setglobal(L, "GetFrameTime");

	lua_newtable(L);
	lua_setglobal(L, "luaugame");

	if (argc > 1) {
		const char* path = TextFormat("%s/main.luau", argv[1]);
		if (luaugame_dofile(L, path) == LUA_OK) {
		} else {
		}
	} else {
		if (luaugame_dofile(L, "main.luau") == LUA_OK) {
		} else {
		}
	}

	luaL_sandbox(L);

	lua_State* T = lua_newthread(L);
	luaL_sandboxthread(T);

	lua_getglobal(T, "luaugame");
	if (lua_istable(T, -1)) {
	} else {
	}

	luaugame_setup(T);

	while (!WindowShouldClose()) {
		if (luaugame_State.UpdateFuncExists)
			luaugame_update(T);
		BeginDrawing();
			if (luaugame_State.DrawFuncExists)
				luaugame_draw(T);
		EndDrawing();
	}

	CloseWindow();

	lua_close(L);

	return 0;
}
