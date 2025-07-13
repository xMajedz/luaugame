-- highly copied from game-premake
project "raylib"
	kind "StaticLib"
    	language "C"
	location "vendor/raylib"
	defines {"PLATFORM_DESKTOP", "GRAPHICS_API_OPENGL_33"}
    	filter "system:linux"
        	defines {"_GLFW_X11"}
        	links {"pthread", "m", "dl", "rt", "x11"}

    	files {"vendor/raylib/src/*.c"}
    	targetdir "vendor/raylib/bin/release"
	removefiles {"vendor/raylib/src/rcore_*.c"}
	includedirs {"vendor/raylib/src", "vendor/raylib/src/external/glfw/include"}
