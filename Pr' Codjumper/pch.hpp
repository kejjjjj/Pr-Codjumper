// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

constexpr auto GAME_NAME = "Call of Duty 4";
constexpr auto MODULE_NAME = "iw3mp.exe";

#define PI 3.14159265f
#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorAddAll(a,b,c)		((c)[0]=(a)[0]+(b),(c)[1]=(a)[1]+(b),(c)[2]=(a)[2]+(b))
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define VectorScaleAll(a,b,c)	((c)[0]=(a)[0]*(b),(c)[1]=(a)[1]*(b),(c)[2]=(a)[2]*(b))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define VectorClear( a )              ( ( a )[0] = ( a )[1] = ( a )[2] = 0 )

#define ANGLE2SHORT( x )  ( (int)( ( x ) * 65536 / 360 ) & 65535 )
#define SHORT2ANGLE( x )  ( ( x ) * ( 360.0f / 65536 ) )

#define DEG2RAD(a) (((a) * M_PI) / 180.0F)
#define RAD2DEG(a) (((a) * 180.0f) / M_PI)
#define RAD2SHORT(a) ((a) * (32768.f / (float)M_PI))
#define SHORT2RAD(a) ((a) * ((float)M_PI / 32768.f))
#define SHORT2DEG(a) (((a) / 32768.f) * 180.0f)

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <Windows.h>
#include <string>
#include <sstream>
#include <thread>
#include <optional>
#include <format>
#include <iostream>
#include <TlHelp32.h>
#include <Psapi.h>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <list>
#include <timeapi.h>
#include <filesystem>
#include <fstream>
#include <direct.h>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

//IMGUI
#include "imGui/imgui.h"
#include "imGui/imgui_impl_dx9.h"
#include "imGui/imgui_impl_win32.h"
#include "imGui/imgui_internal.h"
#include "imGui/imgui_stdlib.h"

#include "sys_tools.hpp"


#include "mh/MinHook.h"

#include "entrypoint.hpp"
#include "resources.hpp"

#include "fs_utilities.hpp"
#include "typedefs.hpp"
#include "koukku.hpp"
#include "errors.hpp"
#include "dvar.hpp"

#include "cg_local.hpp"
#include "cg_offsets.hpp"

#include "e_utilities.hpp"

#include "cg_init.hpp"
#include "cg_hooks.hpp"

#include "r_renderer.hpp"
#include "r_drawtools.hpp"

#include "sv_ccmds.hpp"

#include "scr_main.hpp"
#include "scr_utilities.hpp"
#include "scr_functions.hpp"


#include "com_channel.hpp"

using namespace std::chrono_literals;


#endif //PCH_H