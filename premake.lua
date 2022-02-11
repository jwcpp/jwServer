--solution "jwEngine"
workspace "jwServer"
configurations { "Debug", "Release" }
platforms {'x32', 'x64'}
--If no system is specified, Premake will identify and target the current operating system.
--system { "windows", "linux" }

filter "system:windows"
	defines {'_CRT_SECURE_NO_WARNINGS'}
	characterset ("MBCS") --Multi-byte Character Set; currently Visual Studio only
	

-- Debug and Release
filter { "configurations:Debug" }
	defines { "DEBUG", "_DEBUG" }
	symbols "On"
filter { "configurations:Release" }
	defines { "NDEBUG" }
    optimize "On"

-- x32 and x64
filter { "platforms:x32" }
	architecture "x86"
filter { "platforms:x64" }
	architecture "x86_64"


project "libcore"
	location "src"
	includedirs{
		"dependencies",
		"dependencies/asio/include",
		"dependencies/openssl"
	}
	language "C++"
	cppdialect "c++17"
	kind "StaticLib"
	targetdir "libs"
	objdir "obj/core"
	local codedir = "src/core";
	files { codedir.."/*.h"}
	files { codedir.."/*.c", codedir.."/*.cc", codedir.."/*.cpp"}
	--removefiles {codedir .. "Database/*.cpp"}
	removefiles {codedir .. "/DumpGdb.cpp"}
	filter "configurations:Debug"
		targetname "d_libcore"
	filter "configurations:Release"
		targetname "r_libcore"

project "web"
	location "src"
	includedirs{
		"dependencies",
		"dependencies/asio/include",
		"dependencies/openssl",
		"src/core",
		"src/common"
	}
	language "C++"
	cppdialect "c++17"
	kind "ConsoleApp"
	targetdir "bin"
	objdir "obj/web"
	local codedir = "src/web";
	files { codedir.."/*.h",codedir.."/*.c", codedir.."/*.cc", codedir.."/*.cpp"}
	files { "src/common/*.*"}
	removefiles {codedir .. "/DumpGdb.cpp"}
	targetname "web"
	libdirs{"libs"}
	filter "configurations:Debug"
		links {'d_libcore'}
	filter "configurations:Release"
		links {'r_libcore'}

project "game"
	location "src"
	includedirs{
		"dependencies",
		"dependencies/asio/include",
		"dependencies/openssl",
		"src/core",
		"src/common",
		"src/db_share"
	}
	language "C++"
	cppdialect "c++17"
	kind "ConsoleApp"
	targetdir "bin"
	objdir "obj/game"
	local codedir = "src/game";
	files { codedir.."/*.h",codedir.."/*.c", codedir.."/*.cc", codedir.."/*.cpp"}
	files { "src/common/*.*"}
	files { "src/db_share/*.*"}
	removefiles {codedir .. "/DumpGdb.cpp"}
	targetname "game"
	libdirs{"libs"}
	filter "configurations:Debug"
		links {'d_libcore'}
	filter "configurations:Release"
		links {'r_libcore'}

project "db"
	location "src"
	includedirs{
		"dependencies",
		"dependencies/asio/include",
		"dependencies/openssl",
		"src/core",
		"src/common",
		"src/db_share"
	}
	language "C++"
	cppdialect "c++17"
	kind "ConsoleApp"
	targetdir "bin"
	objdir "obj/db"
	local codedir = "src/db";
	files { "src/core/Database/*.*"}
	files { codedir.."/*.h",codedir.."/*.c", codedir.."/*.cc", codedir.."/*.cpp"}
	files { "src/common/*.*"}
	files { "src/db_share/*.*"}
	removefiles {codedir .. "/DumpGdb.cpp"}
	targetname "db"
	libdirs{"libs"}
	--links
	links{'libmysqlclient'}
	filter "configurations:Debug"
		links {'d_libcore'}
	filter "configurations:Release"
		links {'r_libcore'}