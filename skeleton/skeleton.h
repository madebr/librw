namespace sk {

#ifdef RW_PS2
#define SKEL_PLATFORM PLATFORM_PS2
#define SKEL_DEVICE ps2
//#define SKEL_HALFPIXEL
#elif defined(RW_GL3)
#define SKEL_PLATFORM PLATFORM_GL3
#define SKEL_DEVICE gl3
#elif defined(RW_D3D9)
#define SKEL_PLATFORM PLATFORM_D3D9
#define SKEL_DEVICE d3d
//#define SKEL_HALFPIXEL
#else
#define SKEL_PLATFORM PLATFORM_NULL
#define SKEL_DEVICE null
#endif

using namespace rw;

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

struct Im2VertexBuffer {
	char *data;
	int elemSize;
	int size;
};

struct Im2VertexBase
{
	rw::V3d pos;
	rw::float32 cameraZ;
	rw::float32 recipCameraZ;
	rw::RGBA color;
	rw::TexCoords texCoord;
};

struct Globals
{
	uint32 engineOpenPlatform;
	void *engineStartParams;
	bool32 halfPixel;
	void (*setMousePositionCb)(int x, int y);

	void (*vert2DReAlloc)(Im2VertexBuffer *buffer, size_t nb, uint32 hint);
	void (*setVert2DInd)(Im2VertexBuffer *buffer, size_t index, Im2VertexBase *data);
	void (*setVert2Dcolor)(Im2VertexBuffer *buffer, size_t index, rw::RGBA *color);

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

#ifdef RW_OPENGL
namespace gl3 {
int main(int argc, char **argv);
}
#endif
#ifdef RW_D3D9
namespace d3d {
int main(int argc, char **argv);
}
#endif
}

sk::EventStatus AppEventHandler(sk::Event e, void *param);

#include "imgui/imgui.h"
#include "imgui/imgui_impl_rw.h"
