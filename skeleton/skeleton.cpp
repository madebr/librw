#include <rw.h>
#include "skeleton.h"

namespace sk {

Globals globals;
Args args;

bool
InitRW(void)
{
	if(!rw::Engine::init())
		return false;
	if(AppEventHandler(sk::PLUGINATTACH, nil) == EVENTERROR)
		return false;
	if(!rw::Engine::open(sk::globals.engineOpenPlatform))
		return false;
	if(!rw::Engine::start(sk::globals.engineStartParams))
		return false;

	rw::Image::setSearchPath("./");
	return true;
}

void
TerminateRW(void)
{
	// TODO: delete all tex dicts
	rw::Engine::stop();
	rw::Engine::close();
	rw::Engine::term();
}

Camera*
CameraCreate(int32 width, int32 height, bool32 z)
{
	Camera *cam;
	cam = Camera::create();
	cam->setFrame(Frame::create());
	cam->frameBuffer = Raster::create(width, height, 0, Raster::CAMERA);
	cam->zBuffer = Raster::create(width, height, 0, Raster::ZBUFFER);
	return cam;
}

void
CameraSize(Camera *cam, Rect *r)
{
	if(cam->frameBuffer){
		cam->frameBuffer->destroy();
		cam->frameBuffer = nil;
	}
	if(cam->zBuffer){
		cam->zBuffer->destroy();
		cam->zBuffer = nil;
	}
	cam->frameBuffer = Raster::create(r->w, r->h, 0, Raster::CAMERA);
	cam->zBuffer = Raster::create(r->w, r->h, 0, Raster::ZBUFFER);
}

EventStatus
EventHandler(Event e, void *param)
{
	EventStatus s;
	s = AppEventHandler(e, param);
	if(e == QUIT){
		globals.quit = 1;
		return EVENTPROCESSED;
	}
	if(s == EVENTNOTPROCESSED)
		switch(e){
		case RWINITIALIZE:
			return InitRW() ? EVENTPROCESSED : EVENTERROR;
		case RWTERMINATE:
			TerminateRW();
			return EVENTPROCESSED;
		default:
			break;
		}
	return s;
}

}

namespace sk{
namespace null {
int
main(int argc, char **argv){
	fprintf(stderr, "%s\n", dbgsprint(rw::ERR_PLATFORM, rw::PLATFORM_NULL));
	return 1;
}
}
}

int
main(int argc, char **argv)
{
	int(*mainF)(int argc, char **argv) = sk::SKEL_DEVICE::main;
	for(int i=1;i<argc;++i) {
		if (!strcmp("-null", argv[i])) {
			mainF = sk::null::main;
		}
#ifdef RW_OPENGL
		else if (!strcmp("-gl3", argv[i])) {
			mainF = sk::gl3::main;
		}
#endif
		else if (!strcmp("-d3d", argv[i])) {
#ifdef RW_D3D9
			mainF = sk::d3d::main;
#else
			fprintf(stderr, "%s\n", dbgsprint(rw::ERR_PLATFORM, rw::PLATFORM_D3D9));
			return 1;
#endif
		}
	}
	return mainF(argc, argv);
}
