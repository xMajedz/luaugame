#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <memory>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "raylib.h"

struct Bytecode
{
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
			return 1;
		}
		
		return 0;
	};
  
private:
	char* m_data;
	size_t m_size;
};

struct State {
	bool CustomInitWindow;

    int setup_ref = 0;
    int update_ref = 0;
    int draw_ref = 0;
} luaugame_State;

auto luaugame_dostring(lua_State* L, const char* string, const char* chunkname) -> int
{
	Bytecode bytecode(string);
	return bytecode.execute(L, chunkname);
}

auto luaugame_dostring(lua_State* L, const char* string) -> int
{
	return luaugame_dostring(L, string, "=dostring");
}

auto luaugame_dofile(lua_State* L, const char* file_name) -> int
{
    if (!FileExists(file_name)) {
	    return 1;
	}
	
	lua_State* T = lua_newthread(L);
	luaL_sandboxthread(T);
	
	char* source = LoadFileText(file_name);
	int status = luaugame_dostring(L, source, "=dofile");
	UnloadFileText(source);
	return status;
}


auto luaugame_InitWindow(lua_State* L) -> int
{
	const char* title = lua_tostring(L, -1);
	int h = lua_tonumber(L, -2);
	int w = lua_tonumber(L, -3);
	InitWindow(w, h, title);

	luaugame_State.CustomInitWindow = true;

	return 1;
}

auto luaugame_ClearBackground(lua_State* L) -> int
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

auto luaugame_DrawText(lua_State* L) -> int
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

auto luaugame_DrawCircle(lua_State* L) -> int
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

auto luaugame_GetFrameTime(lua_State* L) -> int
{
	lua_pushnumber(L, GetFrameTime());
	return 1;
}

auto luaugame_GetScreenSize(lua_State* L) -> int
{
	lua_pushnumber(L, GetScreenWidth());
	printf("%d", GetScreenWidth());
	lua_pushnumber(L, GetScreenHeight());
	printf("%d", GetScreenHeight());
	return 2;
}

void luaugame_setup(lua_State* L)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, luaugame_State.setup_ref);
	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
	    printf("%s\n", lua_tostring(L, -1));
	}
	
	if (!luaugame_State.CustomInitWindow)
		InitWindow(800, 450, "untitled luaugame");
}

void luaugame_update(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, luaugame_State.update_ref);
  lua_pushnumber(L, GetFrameTime());
  if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
	  printf("%s\n", lua_tostring(L, -1));
  }
}

void luaugame_draw(lua_State* L)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, luaugame_State.draw_ref);
  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
	  printf("%s\n", lua_tostring(L, -1));
  }
}

void luaugame_nogame(lua_State* L)
{
    const char* nogame =
	"function luaugame.setup()  end\n"
	"function luaugame.update() end\n"
	"function luaugame.draw()   end\n";
    luaugame_dostring(L, nogame);
}

int main(int argc, char** argv)
{
    std::unique_ptr<lua_State, void (*)(lua_State*)> ML(luaL_newstate(), lua_close);
	const auto& L = ML.get();
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
		if (luaugame_dofile(L, path) != LUA_OK) {
		    luaugame_nogame(L);
		}
	} else {
	    luaugame_nogame(L);
	}

	luaL_sandbox(L);

	lua_getglobal(L, "luaugame");
	if (!lua_istable(L, -1)) {
		printf("luaugame not table\n");
		return 1;
	}
	
	lua_getfield(L, -1, "setup");
	if (!lua_isfunction(L, -1)) {
	    printf("luaugame.setup is not a function");
		return 1;
	}
		
	luaugame_State.setup_ref = lua_ref(L, -1);

	lua_getfield(L, -2, "update");
	if (!lua_isfunction(L, -1)) {
	    printf("luaugame.update is not a function");
		return 1;
	}
	luaugame_State.update_ref = lua_ref(L, -1);

	lua_getfield(L, -3, "draw");
	if (!lua_isfunction(L, -1)) {
	    printf("luaugame.draw is not a function");
		return 1;
	}
	luaugame_State.draw_ref = lua_ref(L, -1);
	
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
}
