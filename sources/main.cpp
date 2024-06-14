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

	const char* chunkname = "=luaugame1";

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
	const char* chunkname = "=luaugame2";


	bytecode.data = luau_compile(source, strlen(source), NULL, &bytecode.size);

	free(source);

	int result = luau_load(L, chunkname, bytecode.data, bytecode.size, 0);

	free(bytecode.data);

	if (result != LUA_OK)
		return 1;

	lua_resume(L, NULL, 0);

	return 0;
}

int luaugame_ClearBackground(lua_State* L)
{
	ClearBackground(RED);
	return 1;
}

int main()
{
	lua_State* L = luaL_newstate();

	lua_pushcfunction(L, luaugame_ClearBackground, "ClearBackground");
	lua_setglobal(L, "ClearBackground");

	if (luaugame_dostring(L, "luaugame = {}") == LUA_OK) {

	} else {

	}

	if (luaugame_dofile(L, "main.luau") == LUA_OK) {

	} else {

	}

	lua_getglobal(L, "luaugame");
	if (lua_istable(L, -1)) {
	} else {
	}

	lua_pushstring(L, "setup");
	lua_gettable(L, -2);
	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
		} else {
		}
	} else {
		printf("setup not");
	}

	InitWindow(1280, 720, "luaugame");

	while (!WindowShouldClose()) {
		lua_pushstring(L, "update");
		lua_gettable(L, -2);
		if (lua_isfunction(L, -1)) {
			if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
			} else {
			}
		} else {
		}
		BeginDrawing();
			lua_pushstring(L, "draw");
			lua_gettable(L, -2);
			if (lua_isfunction(L, -1)) {
				if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
				} else {
				}
			} else {
			}
		EndDrawing();
	}

	CloseWindow();

	lua_close(L);

	return 0;
}
