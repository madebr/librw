#include <rw.h>
#include <skeleton.h>

using namespace rw;

//
// This is a test to implement T&L in software and render with Im2D
//

#define MAX_LIGHTS 8

struct Directional {
	V3d at;
	RGBAf color;
};
static Directional directionals[MAX_LIGHTS];
static int32 numDirectionals;
static RGBAf ambLight;

static void
enumLights(Matrix *lightmat)
{
	int32 n;
	World *world;

	world = (World*)engine->currentWorld;
	ambLight.red = 0.0;
	ambLight.green = 0.0;
	ambLight.blue = 0.0;
	ambLight.alpha = 0.0;
	numDirectionals = 0;
	// only unpositioned lights right now
	FORLIST(lnk, world->directionalLights){
		Light *l = Light::fromWorld(lnk);
		if(l->getType() == Light::DIRECTIONAL){
			if(numDirectionals >= MAX_LIGHTS)
				continue;
			n = numDirectionals++;
			V3d::transformVectors(&directionals[n].at, &l->getFrame()->getLTM()->at, 1, lightmat);
			directionals[n].color = l->color;
			directionals[n].color.alpha = 0.0f;
		}else if(l->getType() == Light::AMBIENT){
			ambLight.red   += l->color.red;
			ambLight.green += l->color.green;
			ambLight.blue  += l->color.blue;
		}
	}
}

static void
drawAtomic(Atomic *a)
{
	sk::Im2VertexBuffer vert2dbuff;
	V3d *xvert;
	Matrix xform;
	Matrix lightmat;
	Camera *cam = (Camera*)engine->currentCamera;
	Geometry *g = a->geometry;
	MeshHeader *mh = g->meshHeader;
	Mesh *m = mh->getMeshes();
	int32 width = cam->frameBuffer->width;
	int32 height = cam->frameBuffer->height;
	RGBA *prelight;
	V3d *normals;
	TexCoords *texcoords;

	Matrix::mult(&xform, a->getFrame()->getLTM(), &cam->viewMatrix);
	Matrix::invert(&lightmat, a->getFrame()->getLTM());

	enumLights(&lightmat);

	xvert = rwNewT(V3d, g->numVertices, MEMDUR_FUNCTION);
	sk::globals.vert2DReAlloc(&vert2dbuff, g->numVertices, rw::MEMDUR_FUNCTION);

	prelight = g->colors;
	normals = g->morphTargets[0].normals;
	texcoords = g->texCoords[0];

	V3d::transformPoints(xvert, g->morphTargets[0].vertices, g->numVertices, &xform);
	for(int32 i = 0; i < g->numVertices; i++){
		float32 recipZ = 1.0f/xvert[i].z;

		sk::Im2VertexBase im2vert;
		im2vert.pos.x = xvert[i].x * recipZ * width;
		im2vert.pos.y = xvert[i].y * recipZ * height;
		im2vert.pos.z = recipZ * cam->zScale + cam->zShift;
		im2vert.cameraZ = xvert[i].z;
		im2vert.recipCameraZ = recipZ;
		im2vert.color.red = 255;
		im2vert.color.green = 0;
		im2vert.color.blue = 0;
		im2vert.color.alpha = 255;
		im2vert.texCoord.u = texcoords[i].u;
		im2vert.texCoord.v = texcoords[i].u;
		sk::globals.setVert2DInd(&vert2dbuff, i, &im2vert);
	}
	for(int32 i = 0; i < mh->numMeshes; i++){
		for(uint32 j = 0; j < m[i].numIndices; j++){
			int32 idx = m[i].indices[j];
			RGBA col;
			RGBAf colf, color;
			if(prelight)
				convColor(&color, &prelight[idx]);
			else{
				color.red = color.green = color.blue = 0.0f;
				color.alpha = 1.0f;
			}
			color = add(color, ambLight);
			if(normals)
				for(int32 k = 0; k < numDirectionals; k++){
					float32 f = dot(normals[idx], neg(directionals[k].at));
					if(f <= 0.0f) continue;
					colf = scale(directionals[k].color, f);
					color = add(color, colf);
				}
			convColor(&colf, &m[i].material->color);
			color = modulate(color, colf);
			clamp(&color);
			convColor(&col, &color);
			sk::globals.setVert2Dcolor(&vert2dbuff, idx, &col);
		}

		rw::Texture *tex = m[i].material->texture;
		if(tex && tex->raster){
			rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
			rw::SetRenderState(rw::TEXTUREADDRESSU, tex->getAddressU());
			rw::SetRenderState(rw::TEXTUREADDRESSV, tex->getAddressV());
			rw::SetRenderState(rw::TEXTUREFILTER, tex->getFilter());
		}else
			rw::SetRenderStatePtr(rw::TEXTURERASTER, nil);

		im2d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST,
			vert2dbuff.data, g->numVertices, m[i].indices, m[i].numIndices);
	}

	rwFree(xvert);
	rwFree(vert2dbuff.data);
}

void
tlTest(Clump *clump)
{
	FORLIST(lnk, clump->atomics){
		Atomic *a = Atomic::fromClump(lnk);
		drawAtomic(a);
	}
}

static sk::Im2VertexBuffer g_clipVerts;

void
genIm3DTransform(void *vertices, int32 numVertices, Matrix *world)
{
	using namespace rw;
//	using namespace SKEL_DEVICE;
	Im3DVertex *objverts;
	V3d pos;
	Matrix xform;
	Camera *cam;
	int32 i;
	objverts = (Im3DVertex*)vertices;

	cam = (Camera*)engine->currentCamera;
	int32 width = cam->frameBuffer->width;
	int32 height = cam->frameBuffer->height;


	xform = cam->viewMatrix;
	if(world)
		xform.transform(world, COMBINEPRECONCAT);

	sk::globals.vert2DReAlloc(&g_clipVerts, numVertices, MEMDUR_EVENT);

	for(i = 0; i < numVertices; i++){
		V3d::transformPoints(&pos, &objverts[i].position, 1, &xform);

		float32 recipZ = 1.0f/pos.z;
		RGBA c = objverts[i].getColor();

		sk::Im2VertexBase im2vert;
		im2vert.pos.x = pos.x * recipZ * width;
		im2vert.pos.y = pos.y * recipZ * height;
		im2vert.pos.z = recipZ * cam->zScale + cam->zShift;
		im2vert.cameraZ = pos.z;
		im2vert.recipCameraZ = recipZ;
		im2vert.color.red = c.red;
		im2vert.color.green = c.green;
		im2vert.color.blue = c.blue;
		im2vert.color.alpha = c.alpha;
		im2vert.texCoord.u = objverts[i].u;
		im2vert.texCoord.v = objverts[i].v;
		sk::globals.setVert2DInd(&g_clipVerts, i, &im2vert);
	}
}

void
genIm3DRenderIndexed(PrimitiveType prim, void *indices, int32 numIndices)
{
	im2d::RenderIndexedPrimitive(prim, g_clipVerts.data, g_clipVerts.size, indices, numIndices);
}

void
genIm3DEnd(void)
{
	rwFree(g_clipVerts.data);
	g_clipVerts.data = nil;
	g_clipVerts.size = 0;
}
