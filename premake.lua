workspace "ORE"
	configurations { "Debug", "Release" }

project "ORE"
	kind "StaticLib"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"

	files { "**.h", "**.c", "**.cpp", "**.inl", "**.hpp" }#

	filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
