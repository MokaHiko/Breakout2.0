workspace "Breakout2.0"
	architecture "x64"
	configurations {"Debug", "Release"}

outputdir = "%{cfg.buildcfg}-${cfg.system}-%{cfg.architecture}"

includeDir = "Libraries/include"

project "Breakout2.0"
	location "Breakout2.0"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Breakout2.0/Source/Breakout/pch.cpp"
	files
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
	}

	includedirs
	{
		includeDir,
		"breakout2.0/source",
		"breakout2.0/Source/Breakout"
	}
	libdirs
	{
		"Libraries/lib"
	}
	links
	{
		"glfw3.lib",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

	filter "configurations:Debug"
	buildoptions "/MDd"

	filter "configurations:Release"
		buildoptions "/MDd"