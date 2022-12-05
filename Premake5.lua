--used to create file directories and other non-project setup

os.mkdir("Bin/")
os.mkdir("Bin/Shaders/")

os.mkdir("Doc")

os.mkdir("Assets/EngineSettings/ImGui/")
os.mkdir("Assets/Models/")
os.mkdir("Assets/Sprites/")
os.mkdir("Assets/Scenes/")

workspace "Launcher"
	location "." 
	architecture "x64"
	startproject "Launcher"

	configurations {
		"Debug",
		"Release"
	}

include "Source/Game"
include "Source/External" 
include "Source/Engine" 
include "Source/Launcher" 