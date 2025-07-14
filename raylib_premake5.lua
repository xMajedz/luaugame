-- highly copied from game-premake

project "raylib"
	kind "StaticLib"
    	language "C"
    	files {"vendor/raylib/src/*.c"}
	removefiles {"vendor/raylib/src/rcore_*.c"}
	includedirs {"vendor/raylib/src", "vendor/raylib/src/external/glfw/include"}
    	targetdir "vendor/raylib/bin/release"
	defines {"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}
	
    	filter {"system:linux or system:windows", "toolset:mingw"}
		links {"winmm", "gdi32", "opengl32"}
		targetextension ".exe"
	
    	filter {"system:linux", "toolset:gcc"}
        	defines {"_GLFW_X11"}
        	links {"pthread", "m", "dl", "rt", "x11"}
