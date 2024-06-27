#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "lua.h"
#include "lualib.h"
#include "luacode.h"
#include "raylib.h"

struct Bytecode {
	size_t size;
	char* data;
};

struct State {
	bool UpdateFuncExists = true;
	bool DrawFuncExists = true;
	bool CustomInitWindow;
} luaugame_State;

char* read_file(const char* file_name)
{
	FILE* f;

	f = fopen(file_name, "rb");

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* content = (char*) malloc(fsize + 1);
	fread(content, fsize, 1, f);

	content[fsize] = '\0';

	fclose(f);

	return content;
}

int luaugame_dostring(lua_State* L, const char* string)
{
	Bytecode bytecode = {0};

	const char* chunkname = "=luaugame_dostring";

	bytecode.data = luau_compile(string, strlen(string), NULL, &bytecode.size);

	int result = luau_load(L, chunkname, bytecode.data, bytecode.size, 0);

	free(bytecode.data);

	if (result != LUA_OK)
		return 1;

	lua_resume(L, NULL, 0);

	return 0;
}

int luaugame_dofile(lua_State* L, const char* file_name)
{
	Bytecode bytecode = {0};

	char* source = read_file(file_name);
	const char* chunkname = "=luaugame_dofile";


	bytecode.data = luau_compile(source, strlen(source), NULL, &bytecode.size);

	free(source);

	int result = luau_load(L, chunkname, bytecode.data, bytecode.size, 0);

	free(bytecode.data);

	if (result != LUA_OK)
		return 1;

	lua_resume(L, NULL, 0);

	return 0;
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

	ClearBackground((Color){ color[0], color[1], color[2], color[3] });

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

	DrawText(text, x, y, fontSize, (Color){ color[0], color[1], color[2], color[3] });

	return 1;
}

int main(int argc, char** argv)
{
	lua_State* L = luaL_newstate();

	lua_pushcfunction(L, luaugame_InitWindow, "InitWindow");
	lua_setglobal(L, "InitWindow");

	lua_pushcfunction(L, luaugame_ClearBackground, "ClearBackground");
	lua_setglobal(L, "ClearBackground");

	lua_pushcfunction(L, luaugame_DrawText, "DrawText");
	lua_setglobal(L, "DrawText");

	if (luaugame_dostring(L, "luaugame = {}") == LUA_OK) {
	} else {
	}

	if (argc > 1) {
		if (luaugame_dofile(L, argv[0]) == LUA_OK) {
		} else {
		}
	} else {
		if (luaugame_dofile(L, "./main.luau") == LUA_OK) {
		} else {
		}
	}

	lua_getglobal(L, "luaugame");
	if (lua_istable(L, -1)) {
	} else {
	}
	
	luaugame_setup(L);

	while (!WindowShouldClose()) {
		if (luaugame_State.UpdateFuncExists)
			luaugame_update(L);
		BeginDrawing();
			if (luaugame_State.DrawFuncExists)
				luaugame_draw(L);
		EndDrawing();
	}

	CloseWindow();

	lua_close(L);

	return 0;
}
