#include "lua.h"
#include "lualib.h"
#include "luacode.h"

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char* data; size_t size; } bytecode_t;

static bytecode_t load_bytecode(const char* string)
{
    bytecode_t code;
    code.data = luau_compile(string, strlen(string), NULL, &code.size);
    return code;
}

static void unload_bytecode(bytecode_t code)
{
    free(code.data);
}

int luaugame_loadstring(lua_State* L, const char* string, const char* chunkname)
{
    bytecode_t code = load_bytecode(string);
    int res = luau_load(L, chunkname, code.data, code.size, 0);
    unload_bytecode(code);
    return res;
}

int  luaugame_loadfile(lua_State* L, const char* filename, const char* chunkname)
{
    if (!FileExists(filename)) {
        return 1;
    }

    char* source = LoadFileText(filename);
    int status = luaugame_loadstring(L, source, chunkname);
    UnloadFileText(source);

    return status;
}

int luaugame_InitWindow(lua_State* L)
{
    int w = lua_tonumber(L, 1);
    int h = lua_tonumber(L, 2);
    const char* title = lua_tostring(L, 3);
	
    InitWindow(w, h, title);

    return 0;
}

int luaugame_ClearBackground(lua_State* L)
{
    if (!lua_istable(L, 1))
        return 1;

    Color color;

    lua_rawgeti(L, 1, 1);
    color.r = lua_tonumber(L, -1);
    lua_rawgeti(L, 1, 2);
    color.g = lua_tonumber(L, -1);
    lua_rawgeti(L, 1, 3);
    color.b = lua_tonumber(L, -1);
    lua_rawgeti(L, 1, 4);
    color.a = lua_tonumber(L, -1);

    ClearBackground(color);

    return 0;
}

int luaugame_DrawText(lua_State* L)
{
    const char* text = lua_tostring(L, 1);
    lua_Number x = lua_tonumber(L, 2);
    lua_Number y = lua_tonumber(L, 3);
    lua_Number fontSize = lua_tonumber(L, 4);
    
    Color color;

    lua_rawgeti(L, 5, 1);
    color.r = lua_tounsigned(L, -1);
    lua_rawgeti(L, 5, 2);
    color.g = lua_tounsigned(L, -1);
    lua_rawgeti(L, 5, 3);
    color.b = lua_tounsigned(L, -1);
    lua_rawgeti(L, 5, 4);
    color.a = lua_tounsigned(L, -1);

    DrawText(text, x, y, fontSize, color);

    return 0;
}

int luaugame_DrawCircle(lua_State* L)
{
    lua_Number posX = lua_tonumber(L, 1);
    lua_Number posY = lua_tonumber(L, 2);
    lua_Number radius = lua_tonumber(L, 3);

    Color color;

    lua_rawgeti(L, 4, 1);
    color.r = lua_tounsigned(L, -1);
    lua_rawgeti(L, 4, 2);
    color.g = lua_tounsigned(L, -1);
    lua_rawgeti(L, 4, 3);
    color.b = lua_tounsigned(L, -1);
    lua_rawgeti(L, 4, 4);
    color.a = lua_tounsigned(L, -1);

    
    DrawCircle(posX, posY, radius, color);

    return 0;
}

int luaugame_GetFrameTime(lua_State* L)
{
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

int luaugame_GetScreenWidth(lua_State* L)
{
    lua_pushnumber(L, GetScreenWidth());
    return 1;
}

int luaugame_GetScreenHeight(lua_State* L)
{
    lua_pushnumber(L, GetScreenHeight());
    return 1;
}

int luaugame_GetScreenSize(lua_State* L)
{
    lua_pushnumber(L, GetScreenWidth());
    lua_pushnumber(L, GetScreenHeight());
    return 2;
}

void luaugame_draw(lua_State* L)
{
    lua_getglobal(L, "draw");
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
	    TraceLog(LOG_ERROR, "%s", lua_tostring(L, -1));
        }
    } else {
	lua_pop(L, 1);
    }
}

static const luaL_Reg libluaugame[] = {
    {"InitWindow",      luaugame_InitWindow},
    {"ClearBackground", luaugame_ClearBackground},
    {"DrawText",        luaugame_DrawText},
    {"DrawCircle",      luaugame_DrawCircle},
    {"GetFrameTime",    luaugame_GetFrameTime},

    {"GetScreenWidth",  luaugame_GetScreenWidth},
    {"GetScreenHeight", luaugame_GetScreenHeight},
    {"GetScreenSize",   luaugame_GetScreenSize},

    {NULL, NULL},
};

int luaopen_luaugame(lua_State* L)
{
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, libluaugame);
    return 1;
}

int main(int argc, char* argv[])
{
    lua_State* L = luaL_newstate();
    
    luaL_openlibs(L);

    lua_pushcfunction(L, luaopen_luaugame, NULL);
    lua_call(L, 0, 0);

    luaL_sandbox(L);
    luaL_sandboxthread(L);
    
    if (argc > 1) luaugame_loadfile(L, TextFormat("%s/main.luau", argv[1]), "=loadfile");

    if (lua_gettop(L) && lua_isfunction(L, -1) && lua_pcall(L, 0, 0, 0) != LUA_OK)
        TraceLog(LOG_ERROR, "%s", lua_tostring(L, -1));

    if (!IsWindowReady())
        InitWindow(800, 450, "untitled luaugame");

    while (!WindowShouldClose()) {
        BeginDrawing();
	    luaugame_draw(L);
	EndDrawing();
    }

    CloseWindow();
    
    lua_close(L);
}
