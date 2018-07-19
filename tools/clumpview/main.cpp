#include <rw.h>
#include <skeleton.h>
#include "camera.h"
#include <assert.h>

rw::V3d zero = { 0.0f, 0.0f, 0.0f };
Camera *camera;
struct SceneGlobals {
	rw::World *world;
	rw::Camera *camera;
	rw::Clump *clump;
} Scene;
rw::Texture *tex, *tex2;

void tlTest(rw::Clump *clump);
void genIm3DTransform(void *vertices, rw::int32 numVertices, rw::Matrix *xform);
void genIm3DRenderIndexed(rw::PrimitiveType prim, void *indices, rw::int32 numIndices);
void genIm3DEnd(void);
void initFont(void);
void printScreen(const char *s, float x, float y);

void
Init(void)
{
	sk::globals.windowtitle = "Clump viewer";
	sk::globals.width = 640;
	sk::globals.height = 480;
	sk::globals.quit = 0;
}

bool
attachPlugins(void)
{
	rw::ps2::registerPDSPlugin(40);
	rw::ps2::registerPluginPDSPipes();

	rw::registerMeshPlugin();
	rw::registerNativeDataPlugin();
	rw::registerAtomicRightsPlugin();
	rw::registerMaterialRightsPlugin();
	rw::xbox::registerVertexFormatPlugin();
	rw::registerSkinPlugin();
	rw::registerUserDataPlugin();
	rw::registerHAnimPlugin();
	rw::registerMatFXPlugin();
	rw::registerUVAnimPlugin();
	rw::ps2::registerADCPlugin();
	return true;
}

void
dumpUserData(rw::UserDataArray *ar)
{
	int i;
	printf("name: %s\n", ar->name);
	for(i = 0; i < ar->numElements; i++){
		switch(ar->datatype){
		case rw::USERDATAINT:
			printf("	%d\n", ar->getInt(i));
			break;
		case rw::USERDATAFLOAT:
			printf("	%f\n", ar->getFloat(i));
			break;
		case rw::USERDATASTRING:
			printf("	%s\n", ar->getString(i));
			break;
		}
	}
}

static rw::Frame*
dumpFrameUserDataCB(rw::Frame *f, void*)
{
	using namespace rw;
	int32 i;
	UserDataArray *ar;
	int32 n = UserDataArray::frameGetCount(f);
	for(i = 0; i < n; i++){
		ar = UserDataArray::frameGet(f, i);
		dumpUserData(ar);
	}
	f->forAllChildren(dumpFrameUserDataCB, nil);
	return f;
}

void
dumpUserData(rw::Clump *clump)
{
	printf("Frames\n");
	dumpFrameUserDataCB(clump->getFrame(), nil);
}

static rw::Frame*
getHierCB(rw::Frame *f, void *data)
{
	using namespace rw;
	HAnimData *hd = rw::HAnimData::get(f);
	if(hd->hierarchy){
		*(HAnimHierarchy**)data = hd->hierarchy;
		return nil;
	}
	f->forAllChildren(getHierCB, data);
	return f;
}

rw::HAnimHierarchy*
getHAnimHierarchyFromClump(rw::Clump *clump)
{
	using namespace rw;
	HAnimHierarchy *hier = nil;
	getHierCB(clump->getFrame(), &hier);
	return hier;
}

void
setupAtomic(rw::Atomic *atomic)
{
	using namespace rw;
	// just remove pipelines that we can't handle for now
//	if(atomic->pipeline && atomic->pipeline->platform != rw::platform)
		atomic->pipeline = NULL;

	// Attach hierarchy to atomic if we're skinned
	HAnimHierarchy *hier = getHAnimHierarchyFromClump(atomic->clump);
	if(hier)
		Skin::setHierarchy(atomic, hier);
}

static void
initHierFromFrames(rw::HAnimHierarchy *hier)
{
	using namespace rw;
	int32 i;
	for(i = 0; i < hier->numNodes; i++){
		if(hier->nodeInfo[i].frame){
			hier->matrices[hier->nodeInfo[i].index] = *hier->nodeInfo[i].frame->getLTM();
		}else
			assert(0);
	}
}

void
setupClump(rw::Clump *clump)
{
	using namespace rw;
	HAnimHierarchy *hier = getHAnimHierarchyFromClump(clump);
	if(hier){
		hier->attach();
		initHierFromFrames(hier);
	}

	FORLIST(lnk, clump->atomics){
		rw::Atomic *a = rw::Atomic::fromClump(lnk);
		setupAtomic(a);
	}
}

bool
InitRW(void)
{
	if(!sk::InitRW())
		return false;

	initFont();

	tex = rw::Texture::read("maze", nil);
	tex2 = rw::Texture::read("checkers", nil);

	const char *filename = "teapot.dff";
	if(sk::args.argc > 1)
		filename = sk::args.argv[1];
	rw::StreamFile in;
	if(in.open(filename, "rb") == NULL){
		printf("couldn't open file\n");
		return false;
	}
	rw::findChunk(&in, rw::ID_CLUMP, NULL, NULL);
	Scene.clump = rw::Clump::streamRead(&in);
	assert(Scene.clump);
	in.close();

	// TEST - Set texture to the all materials of the clump
//	FORLIST(lnk, Scene.clump->atomics){
//		rw::Atomic *a = rw::Atomic::fromClump(lnk);
//		for(int i = 0; i < a->geometry->matList.numMaterials; i++)
//			a->geometry->matList.materials[i]->setTexture(tex);
//	}

	Scene.clump->getFrame()->translate(&zero, rw::COMBINEREPLACE);

	dumpUserData(Scene.clump);
	setupClump(Scene.clump);

	Scene.world = rw::World::create();

	rw::Light *ambient = rw::Light::create(rw::Light::AMBIENT);
	ambient->setColor(0.2f, 0.2f, 0.2f);
	Scene.world->addLight(ambient);

	rw::V3d xaxis = { 1.0f, 0.0f, 0.0f };
	rw::Light *direct = rw::Light::create(rw::Light::DIRECTIONAL);
	direct->setColor(0.8f, 0.8f, 0.8f);
	direct->setFrame(rw::Frame::create());
	direct->getFrame()->rotate(&xaxis, 180.0f, rw::COMBINEREPLACE);
	Scene.world->addLight(direct);

	camera = new Camera;
	Scene.camera = sk::CameraCreate(sk::globals.width, sk::globals.height, 1);
	camera->m_rwcam = Scene.camera;
	camera->m_aspectRatio = 640.0f/480.0f;
	camera->m_near = 0.1f;
	camera->m_far = 450.0f;
	camera->m_target.set(0.0f, 0.0f, 0.0f);
	camera->m_position.set(0.0f, -10.0f, 0.0f);
//	camera->setPosition(Vec3(0.0f, 5.0f, 0.0f));
//	camera->setPosition(Vec3(0.0f, -70.0f, 0.0f));
//	camera->setPosition(Vec3(0.0f, -1.0f, 3.0f));
	camera->update();

	Scene.world->addCamera(camera->m_rwcam);
	Scene.clump->world = Scene.world;

	return true;
}

void
im2dtest(void)
{
	int i;
	static struct
	{
		float x, y;
		rw::uint8 r, g, b, a;
		float u, v;
	} vs[4] = {
/*
		{   0.0f,   0.0f,   255, 0, 0, 128,    0.0f, 0.0f },
		{ 640.0f,   0.0f,   0, 255, 0, 128,    1.0f, 0.0f },
		{   0.0f, 480.0f,   0, 0, 255, 128,    0.0f, 1.0f },
		{ 640.0f, 480.0f,   0, 255, 255, 128,  1.0f, 1.0f },
*/
		{   0.0f,   0.0f,   255, 0, 0, 128,    0.0f, 1.0f },
		{ 640.0f,   0.0f,   0, 255, 0, 128,    0.0f, 0.0f },
		{   0.0f, 480.0f,   0, 0, 255, 128,    1.0f, 1.0f },
		{ 640.0f, 480.0f,   0, 255, 255, 128,  1.0f, 0.0f },
	};

	static const int NB = 4;
	sk::Im2VertexBuffer vertBuffer;
	if(vertBuffer.size < NB){
		sk::globals.vert2DReAlloc(&vertBuffer, NB, rw::MEMDUR_NA);
	}
	static short indices[] = {
		0, 1, 2, 3
	};

	float recipZ = 1.0f/Scene.camera->nearPlane;
	for(i = 0; i < 4; i++){
		sk::Im2VertexBase im2vtx;
		im2vtx.pos.x = vs[i].x;
		im2vtx.pos.y = vs[i].y;
		im2vtx.pos.z = rw::im2d::GetNearZ();
		im2vtx.cameraZ = Scene.camera->nearPlane;
		im2vtx.recipCameraZ = recipZ;
		im2vtx.color.red = vs[i].r;
		im2vtx.color.green= vs[i].g;
		im2vtx.color.blue = vs[i].b;
		im2vtx.color.alpha = vs[i].a;
		im2vtx.texCoord.u = vs[i].u;
		im2vtx.texCoord.v = vs[i].v;
		sk::globals.setVert2DInd(&vertBuffer, i, &im2vtx);
	}

	rw::SetRenderStatePtr(rw::TEXTURERASTER, tex2->raster);
	rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
	rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);
	rw::SetRenderState(rw::VERTEXALPHA, 1);
	rw::im2d::RenderIndexedPrimitive(rw::PRIMTYPETRISTRIP,
		vertBuffer.data, 4, &indices, 4);
}

void
im3dtest(void)
{
	int i;
	static struct
	{
		float x, y, z;
		rw::uint8 r, g, b, a;
		float u, v;
	} vs[8] = {
		{ -1.0f, -1.0f, -1.0f,   255, 0, 0, 128,    0.0f, 0.0f },
		{ -1.0f,  1.0f, -1.0f,   0, 255, 0, 128,    0.0f, 1.0f },
		{  1.0f, -1.0f, -1.0f,   0, 0, 255, 128,    1.0f, 0.0f },
		{  1.0f,  1.0f, -1.0f,   255, 0, 255, 128,  1.0f, 1.0f },

		{ -1.0f, -1.0f,  1.0f,   255, 0, 0, 128,    0.0f, 0.0f },
		{ -1.0f,  1.0f,  1.0f,   0, 255, 0, 128,    0.0f, 1.0f },
		{  1.0f, -1.0f,  1.0f,   0, 0, 255, 128,    1.0f, 0.0f },
		{  1.0f,  1.0f,  1.0f,   255, 0, 255, 128,  1.0f, 1.0f },
	};

	const int NB = 8;
	static sk::Im2VertexBuffer vertBuffer;
	sk::globals.vert2DReAlloc(&vertBuffer, NB, rw::MEMDUR_NA);

	static short indices[2*6] = {
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7
	};

	for(i = 0; i < NB; i++){
		sk::Im2VertexBase im2Vertex;
		im2Vertex.pos.x = vs[i].x;
		im2Vertex.pos.y = vs[i].y;
		im2Vertex.pos.z = vs[i].z;
		im2Vertex.color.red = vs[i].r;
		im2Vertex.color.green = vs[i].g;
		im2Vertex.color.blue = vs[i].b;
		im2Vertex.color.alpha = vs[i].a;
		im2Vertex.texCoord.u = vs[i].u;
		im2Vertex.texCoord.v = vs[i].v;
		sk::globals.setVert2DInd(&vertBuffer, i, &im2Vertex);
	}

	rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
	rw::SetRenderState(rw::TEXTUREADDRESS, rw::Texture::WRAP);
	rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::NEAREST);

/*
	genIm3DTransform(verts, 8, nil);
	genIm3DRenderIndexed(rw::PRIMTYPETRILIST, indices, 12);
	genIm3DEnd();
*/
	rw::im3d::Transform(vertBuffer.data, NB, nil);
	rw::im3d::RenderIndexed(rw::PRIMTYPETRILIST, indices, 12);
	rw::im3d::End();
}

void
Draw(float timeDelta)
{
	static rw::RGBA clearcol = { 0x80, 0x80, 0x80, 0xFF };
	camera->m_rwcam->clear(&clearcol, rw::Camera::CLEARIMAGE|rw::Camera::CLEARZ);
	camera->update();
	camera->m_rwcam->beginUpdate();

	Scene.clump->render();
//	im2dtest();
//	tlTest(Scene.clump);
//	im3dtest();
//	printScreen("Hello, World!", 10, 10);

	camera->m_rwcam->endUpdate();
	camera->m_rwcam->showRaster();
}


void
KeyUp(int key)
{
}

void
KeyDown(int key)
{
	switch(key){
	case 'W':
		camera->orbit(0.0f, 0.1f);
		break;
	case 'S':
		camera->orbit(0.0f, -0.1f);
		break;
	case 'A':
		camera->orbit(-0.1f, 0.0f);
		break;
	case 'D':
		camera->orbit(0.1f, 0.0f);
		break;
	case sk::KEY_UP:
		camera->turn(0.0f, 0.1f);
		break;
	case sk::KEY_DOWN:
		camera->turn(0.0f, -0.1f);
		break;
	case sk::KEY_LEFT:
		camera->turn(0.1f, 0.0f);
		break;
	case sk::KEY_RIGHT:
		camera->turn(-0.1f, 0.0f);
		break;
	case 'R':
		camera->zoom(0.1f);
		break;
	case 'F':
		camera->zoom(-0.1f);
		break;
	case sk::KEY_ESC:
		sk::globals.quit = 1;
		break;
	}
}

void
MouseMove(int x, int y)
{
}

void
MouseButton(int buttons)
{
}

sk::EventStatus
AppEventHandler(sk::Event e, void *param)
{
	using namespace sk;
	Rect *r;
	MouseState *ms;

	switch(e){
	case INITIALIZE:
		Init();
		return EVENTPROCESSED;
	case RWINITIALIZE:
		return ::InitRW() ? EVENTPROCESSED : EVENTERROR;
	case PLUGINATTACH:
		return attachPlugins() ? EVENTPROCESSED : EVENTERROR;
	case KEYDOWN:
		KeyDown(*(int*)param);
		return EVENTPROCESSED;
	case KEYUP:
		KeyUp(*(int*)param);
		return EVENTPROCESSED;
	case MOUSEBTN:
		ms = (MouseState*)param;
		MouseButton(ms->buttons);
		return EVENTPROCESSED;
	case MOUSEMOVE:
		ms = (MouseState*)param;
		MouseMove(ms->posx, ms->posy);
		return EVENTPROCESSED;
	case RESIZE:
		r = (Rect*)param;
		// TODO: register when we're minimized
		if(r->w == 0) r->w = 1;
		if(r->h == 0) r->h = 1;

		sk::globals.width = r->w;
		sk::globals.height = r->h;
		// TODO: set aspect ratio
		if(Scene.camera)
			sk::CameraSize(Scene.camera, r);
		break;
	case IDLE:
		Draw(*(float*)param);
		return EVENTPROCESSED;
	}
	return sk::EVENTNOTPROCESSED;
}
