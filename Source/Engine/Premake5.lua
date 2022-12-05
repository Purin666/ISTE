project "ISTE-Core"
	location "%{wks.location}/Local/"
	dependson { "External" }

	language "C++"
	cppdialect "C++17"

	targetdir ("../External/Libs/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../Bin/Intermediate/%{prj.name}/%{cfg.buildcfg}") 

	files {
		"**.h",
		"**.cpp",
		"**.hlsl",
		"**.hlsli",
		"**.hpp",
	}

	includedirs { 
		"../External/Include",
		".", 
	}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on" 

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"  

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		--warnings "Extra"
		--sdlchecks "true"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		--	"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		links {
			"DXGI" 
		}

		defines {
			"ISTE_Core",
			"WIN32",
			"_CRT_SECURE_NO_WARNINGS", 
			"_LIB", 
			"_WIN32_WINNT=0x0601", 
		}

	shadermodel("5.0")
	local shader_dir = "%{wks.location}/Bin/Shaders/"

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")

	filter("files:**_PS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Pixel")

	filter("files:**_VS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Vertex")

	filter("files:**_GS.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Geometry")

	-- Warnings as errors
	shaderoptions({"/WX"})
