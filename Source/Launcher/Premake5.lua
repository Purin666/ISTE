
project "Launcher"
	dependson { "External", "ISTE-Core", "Game" }

	location "%{wks.location}/Local/"  
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	
	targetname("%{prj.name}_%{cfg.buildcfg}")
	targetdir ("../../Bin/")
	debugdir ("../../Bin/")
	objdir ("../../Bin/Intermediate/%{prj.name}/%{cfg.buildcfg}")
	
	files {
		"**.h",
		"**.cpp",
	}

	includedirs {
		"../Engine",
		"../External/Include",
		"../Editor/",
		"../Game"
	}
	libdirs { 
		"../External/Libs/" 
	}

	links { 
		"ISTE-Core",
		"External",
		"Game",
		"d3d11.lib",
		"dxguid.lib",
		"fmodL_vc.lib",
		"fmodstudioL_vc.lib"
	}

	defines { "Launcher" }
	
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