
project "Game"
	dependson { "External", "ISTE-Core" }
	location "%{wks.location}/Local/"  

	kind "StaticLib"

	language "C++"
	cppdialect "C++17"
	
	debugdir "../../Bin/"
	targetdir ("../../Bin/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../Bin/Intermediate/%{prj.name}/%{cfg.buildcfg}")
	
	files {
		"**.h",
		"**.cpp",
	}

	includedirs {
		"../Engine",
		"../External/Include",
		"../Editor/",
		"../Launcher/"
	}
	libdirs { 
		"../External/Libs/" 
	}

	defines { "Game" }
	
	systemversion "latest" 

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {"_RELEASE"}
		runtime "Release"
		optimize "on"