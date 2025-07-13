include "luau_premake5.lua"

project "luaugame"
	kind "WindowedApp"
	libdirs {"raylib/src"}
	includedirs {"luau/Compiler/include", "luau/VM/include", "raylib/src"}
	links {"luaucompiler", "luauast", "luauvm", "raylib"}
	files {"sources/main.cpp"}
	targetdir "build/${prj.cfg}"
