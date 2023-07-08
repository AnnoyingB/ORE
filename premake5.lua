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

newoption {
   trigger     = "api",
   value       = "API",
   description = "Choose a particular API for rendering",
   default     = "opengl",
   category    = "Options",
   allowed = {
      { "opengl",    "OpenGL" },
      { "vulkan",    "Vulkan" },
   }
}

workspace "OREProj"
	configurations { "Debug", "Release", "ExecTest" }
	platforms { "x64" }
	kind "StaticLib"
    architecture "x64"
	startproject "ORE"


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

	includedirs { "OREML/vendor", "OREML/include/OREML", "ORE/include/ORE", "ORE/vendor" }
	libdirs { "OREML/vendor/bin" }
	links { "assimp.lib", "zlibstatic.lib" }

	filter "configurations:Debug or configurations:ExecTest"
		libdirs { "OREML/vendor/bin/Debug" }
		defines { "DEBUG" }
		symbols "On"
	
	filter "configurations:Release"
		libdirs { "OREML/vendor/bin/Release" }
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

	filter "options:api=opengl"
		files { "ORE/**.h", "ORE/**.c", "ORE/**.cpp", "ORE/**.inl", "ORE/**.hpp" }
		removefiles { "ORE/**/Vulkan/**.**" }
	filter "options:api=vulkan"
		files { "ORE/include/ORE/Vulkan/**.**", "ORE/src/Vulkan/**.**", "ORE/**/Window.**", "ORE/include/ORE/orepch.**", "ORE/**/main.**", "ORE/vendor/**.**" }
		removefiles { "ORE/vendor/glad/**.**", "ORE/vendor/KHR/**.**" }
		defines {"API_VULKAN"}
	filter {}
	includedirs { "ORE/vendor", "ORE/include/ORE" }
	libdirs { "ORE/vendor/libs" }
	links
	{
		"freetyped.lib",
		"glfw3.lib",
		"glfw3_mt.lib",
		"glslang.lib",
	}

	filter "options:extensions=oreml"
		includedirs { "OREML/include", "OREML/vendor" }
		libdirs { "OREML/vendor/", "OREML/vendor/bin" }
		links { "OREML", "zlibstatic.lib" }

	filter "configurations:Debug or configurations:ExecTest"
		libdirs { "ORE/vendor/libs/Debug" }
		defines { "DEBUG" }
		symbols "On"

    filter "configurations:Release"
		libdirs { "ORE/vendor/libs/Release" }
		defines { "NDEBUG" }
		optimize "On"
