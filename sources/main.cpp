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

        int load(lua_State* L, const char* chunkname)
	{
             return luau_load(L, chunkname, data(), size(), 0);
	};
  
private:
	char* m_data;
	size_t m_size;
};

struct State {
    bool CustomInitWindow;

    int draw_ref = 0;
} luaugame_State;

auto luaugame_loadstring(lua_State* L, const char* string, const char* chunkname) -> int
{
    Bytecode bytecode(string);
    bytecode.load(L, chunkname);
    return 0;
}

auto luaugame_loadfile(lua_State* L, const char* filename, const char* chunkname) -> int
{
    if (!FileExists(filename)) {
        return 1;
    }

    Bytecode bytecode(filename);
    bytecode.load(L, chunkname);
    	
    char* source = LoadFileText(filename);
    int status = luaugame_loadstring(L, source, chunkname);
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
	lua_pushnumber(L, GetScreenHeight());
	return 2;
}

void luaugame_draw(lua_State* L)
{
    lua_getref(L, luaugame_State.draw_ref);
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            printf("%s\n", lua_tostring(L, -1));
        }
    } else {
	lua_pop(L, 1);
    }
}

static const luaL_Reg libluaugame[] = {
    {"InitWindow", luaugame_InitWindow},
    {"ClearBackground", luaugame_ClearBackground},
    {"DrawText", luaugame_DrawText},
    {"DrawCircle", luaugame_DrawCircle},
    {"GetFrameTime", luaugame_GetFrameTime},
    {"GetScreenSize", luaugame_GetScreenSize},
    {NULL, NULL},
};

auto luaopen_luaugame(lua_State* L) -> int
{
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, libluaugame);
    return 1;
}

static void* lua_alloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
    (void)ud;
    (void)osize;
    if (nsize == 0)
    {
        free(ptr);
        return NULL;
    }
    else
        return realloc(ptr, nsize);
}

int main(int argc, char* argv[])
{
    lua_State* L = lua_newstate(lua_alloc, NULL);
    
    luaL_openlibs(L);

    lua_pushcfunction(L, luaopen_luaugame, NULL);
    lua_call(L, 0, 0);
    
    luaL_sandbox(L);
    luaL_sandboxthread(L);
    
    if (argc > 1) luaugame_loadfile(L, TextFormat("%s/main.luau", argv[1]), "=loadfile");

    if (lua_gettop(L) && lua_isfunction(L, -1) && lua_pcall(L, 0, 0, 0) != LUA_OK)
        printf("%s\n", lua_tostring(L, -1));

    lua_getglobal(L, "draw");
    if (lua_isfunction(L, -1))
        luaugame_State.draw_ref = lua_ref(L, -1);
    
    if (!luaugame_State.CustomInitWindow)
        InitWindow(800, 450, "untitled luaugame");

    while (!WindowShouldClose()) {
        BeginDrawing();
	    luaugame_draw(L);
	EndDrawing();
    }

    CloseWindow();
    
    lua_close(L);
}
