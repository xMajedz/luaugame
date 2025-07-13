workspace "luaugame"
	language "C++"
	configurations {"debug", "release"}
	libdirs {"raylib/src"}

	filter { "configurations:debug" }
		libdirs {"luau/build/debug"}
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:release" }
		libdirs {"luau/build/release"}
		defines { "NDEBUG" }
		optimize "On"

project "luaugame"
	kind "WindowedApp"
	includedirs {"luau/Compiler/include", "luau/VM/include", "raylib/src"}
	links {"luaucompiler", "luauast", "luauvm", "raylib"}
	files {"sources/main.cpp"}
