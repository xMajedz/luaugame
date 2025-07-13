newoption {
	trigger = "luau-path",
	default = "./vendor/luau",
	description = "sets the path to luau distribution.",
}

newoption {
	trigger = "config",
	description = "sets configuration.",
}

local config = _OPTIONS["config"]
local path = _OPTIONS["luau-path"]

workspace "Luau"
	language "C++"

	filter "not options:config"
		configurations {"debug", "release"}

	filter "options:config=debug"
		configurations {"debug"}

	filter "options:config=release"
		configurations {"release"}

	filter "configurations:debug"
		targetdir ( path .. "/build/debug")
		libdirs { path .. "/build/debug"}
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:release"
		targetdir ( path .. "/build/release")
		libdirs { path .. "/build/release"}
		defines { "NDEBUG" }
		optimize "On"


project "Luau.Ast"
	kind "StaticLib"
	cppdialect "C++17"
	includedirs {path .. "/Common/include", path .. "/Ast/include"}
	files {path .. "/Ast/src/*.cpp"}
	targetname "luauast"

project "Luau.Compiler"
	kind "StaticLib"
	cppdialect "C++17"
	includedirs {path .."/Compiler/include", path .. "/Common/include", path .. "/Ast/include"}
	files {path .. "/Compiler/src/*.cpp"}
	targetname "luaucompiler"

project "Luau.VM"
	kind "StaticLib"
	cppdialect "C++11"
	includedirs {path .. "/Common/include", path .. "/VM/include"}
	files {path .. "/VM/src/*.cpp"}
	targetname "luauvm"

project "Luau.Web"
	kind "None"
	filter {"options:os=emscripten"}
	kind "ConsoleApp"
	libdirs {path .. "/build/${prj.cfg}"}
	includedirs {path .. "/Common/include", path .. "/Compiler/include", path .. "/VM/include"}
	files {path .. "/CLI/src/Web.cpp"}
	links {"luauast", "luaucompiler", "luauvm"}
	linkoptions {
		"-sWASM=0",
		"-sSINGLE_FILE=1",
		"-sEXPORTED_FUNCTIONS=['_executeScript']",
		"-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']",
		"-fexceptions",
	}
