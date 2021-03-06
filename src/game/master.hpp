#pragma once

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

#include "gameutils/adaptive_viewport.h"
#include "gameutils/config.h"
#include "gameutils/render.h"
#include "gameutils/texture_atlas.h"
#include "gameutils/tiled_map.h"
#include "graphics/complete.h"
#include "input/complete.h"
#include "interface/gui.h"
#include "interface/window.h"
#include "program/errors.h"
#include "program/exit.h"
#include "reflection/complete.h"
#include "utils/clock.h"
#include "utils/filesystem.h"
#include "utils/format.h"
#include "utils/meta.h"
#include "utils/metronome.h"
#include "utils/poly_storage.h"
#include "utils/random.h"
#include "utils/with.h"

#include <process.hpp>

#ifdef PLATFORM_WINDOWS
#include <windef.h>
#include <winbase.h>
#include <winuser.h>
#include <shellapi.h>
#undef MessageBox
#endif
