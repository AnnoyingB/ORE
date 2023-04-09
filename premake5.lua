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
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	
	filter "platforms:x86"
    	kind "StaticLib"
    	architecture "x32"

  	filter "platforms:x64"
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

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	

project "ORE"
	location "ORE"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"

	files { "ORE/**.h", "ORE/**.c", "ORE/**.cpp", "ORE/**.inl", "ORE/**.hpp" }

	filter "options:extensions=none"
		includedirs { "ORE/vendor", "ORE/include/ORE" }
	filter "options:extensions=oreml"
		includedirs { "ORE/vendor", "ORE/include/ORE", "OREML/include" }
		libdirs { "OREML/x64/Debug" }
		links { "OREML.lib" }

	filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
