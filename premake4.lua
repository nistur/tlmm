--[[
-- insert zlib license here
--]]


--[[
-- Tiny Little Maths Machine solution
----------------------------------------
-- This project contains three targets
-- tlmm - full static library
-- tlmm-lite - library which doesn't support file IO or compiling
-- tlmc - tlmm compiler
--]]
solution "tlmm"
configurations { "Debug", "Release" }
language "C++"
includedirs { "include" }

if os.is("windows") then
defines { "WIN32" }
print("meh!")
end

configuration "Debug"
defines { "DEBUG" }
flags { "Symbols" }
targetdir "build/debug"

configuration "Release"
defines { "NDEBUG" }
flags { "OptimizeSpeed",
	"EnableSSE", 
	"EnableSSE2",
	"FloatFast",
	"ExtraWarnings",
	"FatalWarnings",
	"NoFramePointer"}
targetdir "build/release"

project "tlmm"
files { "include/tlmm.h", "src/tlmm.cpp" }
kind "StaticLib"
defines
{
   "TLMM_HAS_IO",
   "TLMM_COMPILE",
}

project "tlmm-lite"
language "C++"
files { "include/tlmm.h", "src/tlmm.cpp" }
kind "StaticLib"
defines
{
   "TLMM_LEAN",
   "TLMM_FAST_SQRT",
}

project "tlmc"
files { "include/tlmm.h", "src/tlmc.cpp" }
kind "ConsoleApp"
links { "tlmm" }

project "tlmr"
files { "include/tlmm.h", "src/tlmr.cpp" }
kind "ConsoleApp"
targetname "tlmm" -- may as well be called tlmm
links { "tlmm" }

project "tests"
files { "include/tlmm.h", "tests/**.cpp" }
kind "ConsoleApp"
links { "tlmm" }
configuration "Debug"
postbuildcommands("build/debug/tests")
configuration "Release"
postbuildcommands("build/release/tests")
