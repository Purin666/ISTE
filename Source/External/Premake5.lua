project "External"
	location "%{wks.location}/Local/" 
	
	language "C++"
	cppdialect "C++17"

	targetdir ("Libs/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../Bin/Intermediate/%{prj.name}/%{cfg.buildcfg}") 

	files {
		"**.h",
		"**.hpp",
		"**.inl",
		"**.c",
		"**.cpp",
	}
	
	defines { "External" }

	excludes {	}

	includedirs {  
		"Include/",
		"Include/imgui/",
		"Include/imguizmo/",
		"Include/ft2build.h",
		"../Engine/",
	}

	libdirs { "Lib/" }

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on" 

	filter "configurations:Release"
		defines {"_RELEASE"}
		runtime "Release"
		optimize "on" 

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		--warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		--	"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		links {
			"DXGI",
			"dxguid",
		}
		
		defines { "_WIN32_WINNT=0x0601" }