workspace "luaugame"
    language "C++"

	configurations {"release", "debug"}
    location "build"
	targetdir "build"

	includedirs {"vendor/luau/Compiler/include", "vendor/luau/VM/include", "vendor/luau/Ast/include", "vendor/luau/Bytecode/include",  "vendor/raylib/src"}	
	libdirs {"vendor/raylib/src", "vendor/luau/build/release"}

	links {"luaucompiler", "luauast", "luauvm", "luaucommon", "luaucodegen", "luaubytecode", "raylib"}

	filter { "configurations:release" }
	defines { "NDEBUG" }
	optimize "On"
	
	filter {"configurations:debug" }
	defines { "DEBUG" }
	symbols "On"

project "luaugame"
    kind "WindowedApp"
    files {"sources/main.cpp"}

	filter {"system:linux or system:windows", "toolset:mingw"}
    links {"winmm", "gdi32", "opengl32"}
    targetextension ".exe"

	filter {"system:linux"}
	links {"X11"}
