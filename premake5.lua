include "luau_premake5.lua"
include "raylib_premake5.lua"

project "luaugame"
	kind "WindowedApp"
	libdirs {"vendor/raylib/bin/release"}
	includedirs {"vendor/luau/Compiler/include", "vendor/luau/VM/include", "vendor/raylib/src"}
	links {"luaucompiler", "luauast", "luauvm", "raylib"}
	files {"sources/main.cpp"}
	targetdir "build/${prj.cfg}"

    	filter {"system:linux or system:windows", "toolset:mingw"}
		links {"winmm", "gdi32", "opengl32"}
		targetextension ".exe"
