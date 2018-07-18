namespace sk {

#ifdef RW_PS2
#define SKEL_PLATFORM PLATFORM_PS2
#define SKEL_DEVICE ps2
#define RWHALFPIXEL
#elif defined(RW_GL3)
#define SKEL_PLATFORM PLATFORM_GL3
#define SKEL_DEVICE gl3
#elif defined(RW_D3D9)
#define SKEL_PLATFORM PLATFORM_D3D9
#define SKEL_DEVICE d3d
#define RWHALFPIXEL
#else
#define SKEL_PLATFORM PLATFORM_NULL
#endif

using namespace rw;

extern uint32 engineOpenPlatform;
extern void *engineStartParams;

// same as RW skeleton
enum Key
{
	// ascii...

	KEY_ESC   = 128,

	KEY_F1    = 129,
	KEY_F2    = 130,
	KEY_F3    = 131,
	KEY_F4    = 132,
	KEY_F5    = 133,
	KEY_F6    = 134,
	KEY_F7    = 135,
	KEY_F8    = 136,
	KEY_F9    = 137,
	KEY_F10   = 138,
	KEY_F11   = 139,
	KEY_F12   = 140,

	KEY_INS   = 141,
	KEY_DEL   = 142,
	KEY_HOME  = 143,
	KEY_END   = 144,
	KEY_PGUP  = 145,
	KEY_PGDN  = 146,

	KEY_UP    = 147,
	KEY_DOWN  = 148,
	KEY_LEFT  = 149,
	KEY_RIGHT = 150,

	// some stuff ommitted

	KEY_BACKSP = 168,
	KEY_TAB    = 169,
	KEY_CAPSLK = 170,
	KEY_ENTER  = 171,
	KEY_LSHIFT = 172,
	KEY_RSHIFT = 173,
	KEY_LCTRL  = 174,
	KEY_RCTRL  = 175,
	KEY_LALT   = 176,
	KEY_RALT   = 177,

	KEY_NULL,	// unused
	KEY_NUMKEYS,
};

enum EventStatus
{
	EVENTERROR,
	EVENTPROCESSED,
	EVENTNOTPROCESSED
};

enum Event
{
	INITIALIZE,
	RWINITIALIZE,
	RWTERMINATE,
	SELECTDEVICE,
	PLUGINATTACH,
	KEYDOWN,
	KEYUP,
	CHARINPUT,
	MOUSEMOVE,
	MOUSEBTN,
	RESIZE,
	IDLE,
	QUIT
};

struct Globals
{
	const char *windowtitle;
	int32 width;
	int32 height;
	bool32 quit;
};
extern Globals globals;

// Argument to mouse events
struct MouseState
{
	int posx, posy;
	int buttons;	// bits 0-2 are left, middle, right button down
};

struct Args
{
	int argc;
	char **argv;
};
extern Args args;

bool InitRW(void);
void TerminateRW(void);
Camera *CameraCreate(int32 width, int32 height, bool32 z);
void CameraSize(Camera *cam, Rect *r);
EventStatus EventHandler(Event e, void *param);

namespace SKEL_DEVICE {
void SetMousePosition(int x, int y);
int main(int argc, char **argv);
}
}

sk::EventStatus AppEventHandler(sk::Event e, void *param);

#include "imgui/imgui.h"
#include "imgui/imgui_impl_rw.h"
