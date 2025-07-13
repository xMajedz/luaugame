#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "raylib.h"

struct Bytecode
{
private:
	char* m_data;
	size_t m_size;
public:
	Bytecode(std::string string) 
	{
		m_data = luau_compile(string.data(), string.size(), NULL, &m_size);
	};

	~Bytecode() 
	{
		delete m_data;
	};

	const char* data()
	{
		return m_data;
	};

	size_t size()
	{
		return m_size;
	};

	int execute(lua_State* L, const char* chunkname)
	{
		luau_load(L, chunkname, data(), size(), 0);
		if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
			printf("%s\n", lua_tostring(L, -1));
		}
		return 1;
	};
};

struct State {
	bool UpdateFuncExists = true;
	bool DrawFuncExists = true;
	bool CustomInitWindow;
} luaugame_State;

int luaugame_dostring(lua_State* L, const char* string, const char* chunkname)
{
	Bytecode bytecode(string);
	return bytecode.execute(L, chunkname);
}

int luaugame_dostring(lua_State* L, const char* string)
{
	return luaugame_dostring(L, string, "=dostring");
}

int luaugame_dofile(lua_State* L, const char* file_name)
{
	lua_State* T = lua_newthread(L);
	luaL_sandboxthread(T);

	char* source = LoadFileText(file_name);
	int status = luaugame_dostring(L, source, "=dofile");
	UnloadFileText(source);
	return status;
}

void luaugame_setup(lua_State* L)
{
	lua_getglobal(L, "luaugame");
	if (lua_istable(L, -1)) {
		lua_getfield(L, -1, "setup");
		if (lua_isfunction(L, -1)) {
			if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
				printf("%s\n", lua_tostring(L, -1));
			}
		}
	} else {
		printf("luaugame not table\n");
	}

	if (!luaugame_State.CustomInitWindow)
		InitWindow(800, 450, "untitled luaugame");
}

void luaugame_update(lua_State* L)
{
	lua_getfield(L, -1, "update");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, GetFrameTime());
		if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
			printf("%s\n", lua_tostring(L, -1));
		}
	} else {
		luaugame_State.UpdateFuncExists = false;
	}
}

void luaugame_draw(lua_State* L)
{
	lua_getfield(L, -1, "draw");
	if (lua_isfunction(L, -1)) {
		if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
			printf("%s\n", lua_tostring(L, -1));
		}
	} else {
		luaugame_State.DrawFuncExists = false;
	}
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
		luaugame_dofile(L, path);
	} else {
		luaugame_dofile(L, "main.luau");
	}

	luaL_sandbox(L);
	
	luaugame_setup(L);

	if (!luaugame_State.CustomInitWindow)
		InitWindow(800, 450, "untitled luaugame");

	while (!WindowShouldClose()) {
		luaugame_update(L);
		BeginDrawing();
			luaugame_draw(L);
		EndDrawing();
	}

	CloseWindow();

	lua_close(L);
}
