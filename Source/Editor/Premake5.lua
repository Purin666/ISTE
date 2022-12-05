-- Creates a seperate project which will be a editor version of the main engine
workspace "Editor"
	location "../../." 
	architecture "x64"  
	startproject "Editor"

	configurations {
		"Debug",
		"Release"
	}

externalproject "Game"
	location "../../Local"
	kind "WindowedApp"
    language "C++" 

externalproject "ISTE-Core" 
	location "../../Local"
	kind "StaticLib"
    language "C++"

externalproject "External" 
	kind "StaticLib"
    language "C++" 
	location "../../Local"


project "Editor"
	dependson { "External", "ISTE-Core", "Game" }
	location "%{wks.location}/Local/"  

	kind "WindowedApp"

	language "C++"
	cppdialect "C++17"
	
	targetname("%{prj.name}_%{cfg.buildcfg}")
	targetdir ("../../Bin/")
	debugdir "../../Bin/"
	objdir ("../../Bin/Intermediate/%{prj.name}/%{cfg.buildcfg}")
	
	files {
		"**.h",
		"**.cpp"
	}

	includedirs {
		"../Engine",
		"../External/Include",
		"../Launcher/",
		"../Editor",
		"../Game"
	}

	libdirs { 
		"../External/Libs/" 
	}

	links { 
		"ISTE-Core",
		"External",
		"d3d11.lib",
		"dxguid.lib", 
		"fmodL_vc.lib",
		"fmodstudioL_vc.lib"
	}

	defines { "Editor" }
	
	systemversion "latest"  

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		links {
			"freetype_Debug_x64.lib",
			"assimpd.lib"
		}

	filter "configurations:Release"
		defines {"_RELEASE"}
		runtime "Release"
		optimize "on"
		links {
			"freetype_Release_x64.lib",
			"assimp.lib"
		}