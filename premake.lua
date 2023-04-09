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

workspace "ORE"
	configurations { "Debug", "Release" }

project "OREML"
	filter "options:not extensions=oreml"
		kind "None"
	filter "options:extensions=oreml"
		kind "StaticLib"
	
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"

	files { "**.h", "**.c", "**.cpp", "**.inl", "**.hpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	

project "ORE"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"

	files { "**.h", "**.c", "**.cpp", "**.inl", "**.hpp" }

	filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
