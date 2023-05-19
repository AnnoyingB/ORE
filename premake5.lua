newoption {
	trigger = "extensions",
	value = "extension",
	description = "Choose which extension/extensions to use with ORE",
	category = "Options",
	allowed = {
		{ "none", "None" },
		{ "oreml", "OreML" },
	},
	default = "none"
}

workspace "OREProj"
	configurations { "Debug", "Release", "ExecTest" }
	platforms { "x64" }
	kind "StaticLib"
    architecture "x64"


project "OREML"
	location "OREML"
	filter "options:not extensions=oreml"
		kind "None"
	filter "options:extensions=oreml"
		kind "StaticLib"
	
	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"

	files { "OREML/**.h", "OREML/**.c", "OREML/**.cpp", "OREML/**.inl", "OREML/**.hpp" }

	includedirs { "OREML/vendor/assimp/include", "OREML/include/OREML" }
	links { "assimp.dll" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	filter {}


project "ORE"
	location "ORE"
	filter "configurations:ExecTest"
	  kind "ConsoleApp"
	filter "configurations:Debug or configurations:Release"
	  kind "StaticLib"
	filter {}

	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"

	files { "ORE/**.h", "ORE/**.c", "ORE/**.cpp", "ORE/**.inl", "ORE/**.hpp" }
	includedirs { "ORE/vendor", "ORE/include/ORE" }
	libdirs { "ORE/vendor/libs" }
	links
	{
		"glfw3.lib",
		"glfw3_mt.lib",
	}

	filter "options:extensions=oreml"
		includedirs { "OREML/include" }
		libdirs { "OREML/x64/Debug" }
		links { "OREML.lib" }

	filter "configurations:Debug or configurations:ExecTest"
      defines { "DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
