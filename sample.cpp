#include "Graphics.h"
#include "math.h"
#include "FileLoaders.h"

#define SHADOWMAP_RES 256

Graphics::Index			gLightmapMVP;

Graphics::FrameBuffer*  gLightmapFBO;
Graphics::Texture*		gLightmapDepth;
Graphics::Texture*		gLightmapColor;

Graphics::Shader*		gLightmapDrawShader;
Graphics::Shader*		gLightmapBlitShader;
Graphics::Index			gLightmapFboAttachment;
Graphics::VertexLayout*	gLightmapMesh;
Graphics::VertexLayout* gLightmapSkullLayout;
Graphics::VertexLayout* gLightmapPlaneLayout;

struct PCMState {
	Graphics::Shader* shader;
	Graphics::Index modelIndex;
	Graphics::Index shadowIndex;
	Graphics::Index viewIndex;
	Graphics::Index projectionIndex;
	Graphics::Index albedoIndex;
	Graphics::Index lightmapIndex;
	Graphics::Index normalIndex;
	Graphics::Index lightDirection;
	Graphics::Index lightColor;
	Graphics::Index viewPos;
	Graphics::Index ambientStrength;
	Graphics::Index ambientOnly;
	Graphics::VertexLayout* skullMesh;
	Graphics::VertexLayout* planeMesh;
};

PCMState* gPCMState;
PCMState* gLitNoPCM;
PCMState* gLitWithPCM;

Graphics::Texture* gPlaneTextureAlbedo;
Graphics::Texture* gPlaneTextureNormal;

Graphics::Shader* gHemiShader;
Graphics::VertexLayout* gHemiSkullMesh;
Graphics::Texture* gSkullTextureAlbedo;
Graphics::Texture* gSkullTextureNormal;
Graphics::Index gAlbedoIndex;
Graphics::Index gNormalIndex;
Graphics::Index gModelIndex;
Graphics::Index gViewIndex;
Graphics::Index gHemiTop;
Graphics::Index gHemiBottom;
Graphics::Index gProjectionIndex;
Graphics::Index gLightDirection;
Graphics::Index gLightColor;
Graphics::Index gLightAmbientOnly;
Graphics::Index gViewPos;

#define NUM_BUFFERS 10
Graphics::Buffer* gBuffers[NUM_BUFFERS];

bool ShowDepth = true;
bool IsRunning = true;
float cameraRadius;
float cameraHeight;
vec3 cameraTarget;
float camTime;
float lightTime;
float lightDir;
bool enablePCM;
bool lastPCM;
float ambientOnly;

TextFile* blit_depth_vShader;
TextFile* blit_depth_fShader;
TextFile* lightmap_vShader;
TextFile* lightmap_fShader;
TextFile* lit_vShader;
TextFile* lit_fShader;
TextFile* lit_pcm_fShader;
TextFile* hemi_vShader;
TextFile* hemi_fShader;
MeshFile* skullMesh;
MeshFile* planeMesh;
TextureFile* skullNormal;
TextureFile* planeAlbedo;
TextureFile* planeNormal;
TextureFile* skullAlbedo;
u32 numFilesToLoad;
bool isFinishedInitializing;

Graphics::Device* globalDevice;

#ifndef GraphicsAssert
void Win32Assert(bool cond, const char* msg) {
	if (!cond) {
		char* devnull = (char*)0;
		*devnull = 'a';
	}
}
#define GraphicsAssert(x, y) Win32Assert(x, y);
#endif

void Initialize(Graphics::Dependencies* platform, Graphics::Device* gfx) {
	IsRunning = true;
	isFinishedInitializing = false;
	ambientOnly = 0.0f;

	cameraRadius = 7.0f;
	cameraHeight = 3.0f;
	camTime = lightTime = 0.0f;
	lightDir = 1.0f;
	cameraTarget.y = 0.0f;

	globalDevice = gfx;
	gLitNoPCM = (PCMState*)gfx->Allocate(sizeof(PCMState));
	gLitWithPCM = (PCMState*)gfx->Allocate(sizeof(PCMState));

	gLightmapFBO = gfx->CreateFrameBuffer();
	gLightmapColor = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, SHADOWMAP_RES, SHADOWMAP_RES);
	gLightmapDepth = gfx->CreateTexture(Graphics::TextureFormat::Depth, SHADOWMAP_RES, SHADOWMAP_RES, 0, Graphics::TextureFormat::Depth, false);
	
	enablePCM = false;
	lastPCM   = false;
	gPCMState = gLitNoPCM;
	gLightmapFBO->AttachDepth(*gLightmapDepth, false);

	//gLightmapFBO->AttachColor(*gLightmapColor);
	GraphicsAssert(gLightmapFBO->IsValid(), "Invalid fbo");

	numFilesToLoad = 15;
	LoadText("assets/blit-depth.vert", [](const char* path, TextFile* file) {
		blit_depth_vShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/blit-depth.frag", [](const char* path, TextFile* file) {
		blit_depth_fShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/lightmap.vert", [](const char* path, TextFile* file) {
		lightmap_vShader = file;
		GraphicsAssert(lightmap_vShader->length != 0, "Empty lightmap.vert");
		GraphicsAssert(lightmap_vShader->text != 0, "No text pointer in lightmap.vert");
		GraphicsAssert(*lightmap_vShader->text != 0, "Empty string in lightmap.vert");
		numFilesToLoad -= 1;
		});
	LoadText("assets/lightmap.frag", [](const char* path, TextFile* file) {
		lightmap_fShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/lit.vert", [](const char* path, TextFile* file) {
		lit_vShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/lit.frag", [](const char* path, TextFile* file) {
		lit_fShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/lit-pcm.frag", [](const char* path, TextFile* file) {
		lit_pcm_fShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/hemi.vert", [](const char* path, TextFile* file) {
		hemi_vShader = file;
		numFilesToLoad -= 1;
		});
	LoadText("assets/hemi.frag", [](const char* path, TextFile* file) {
		hemi_fShader = file;
		numFilesToLoad -= 1;
		});
	LoadMesh("assets/skull.mesh", [](const char* path, MeshFile* file) {
		skullMesh = file;
		numFilesToLoad -= 1;
		});
	LoadMesh("assets/plane.mesh", [](const char* path, MeshFile* file) {
		planeMesh = file;
		numFilesToLoad -= 1;
		});
	LoadTexture("assets/Skull_AlbedoSpec.texture", [](const char* path, TextureFile* file) {
		skullAlbedo = file;
		numFilesToLoad -= 1;
		});
	LoadTexture("assets/Plane_AlbedoSpec.texture", [](const char* path, TextureFile* file) {
		planeAlbedo = file;
		numFilesToLoad -= 1;
		});
	LoadTexture("assets/Plane_Normal.texture", [](const char* path, TextureFile* file) {
		planeNormal = file;
		numFilesToLoad -= 1;
		});
	LoadTexture("assets/Skull_Normal.texture", [](const char* path, TextureFile* file) {
		skullNormal = file;
		numFilesToLoad -= 1;
		});
}

void FinishInitializing(Graphics::Device* gfx) {
	GraphicsAssert(lightmap_vShader->length != 0, "2c Empty lightmap.vert");
	GraphicsAssert(lightmap_vShader->text != 0, "2 No text pointer in lightmap.vert");
	GraphicsAssert(*lightmap_vShader->text != 0, "2 Empty string in lightmap.vert");
	gLightmapDrawShader = gfx->CreateShader(lightmap_vShader->text, lightmap_fShader->text);
	ReleaseText(lightmap_vShader);
	ReleaseText(lightmap_fShader);
	
	gLightmapBlitShader = gfx->CreateShader(blit_depth_vShader->text, blit_depth_fShader->text);
	ReleaseText(blit_depth_vShader);
	ReleaseText(blit_depth_fShader);
	gLightmapFboAttachment = gLightmapBlitShader->GetUniform("fboAttachment");

	gLightmapSkullLayout = gfx->CreateVertexLayout();
	Graphics::Index lightmapPositionAttrib = gLightmapDrawShader->GetAttribute("position");
	

	gLightmapPlaneLayout = gfx->CreateVertexLayout();

	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	Graphics::Buffer* lightmapMesh = gfx->CreateBuffer(quadVertices, sizeof(float) * 20);
	Graphics::BufferView lightmapMeshPos(3, sizeof(float) * 5, Graphics::BufferType::Float32, 0);
	Graphics::BufferView lightmapMeshTex(2, sizeof(float) * 5, Graphics::BufferType::Float32, sizeof(float) * 3);
	Graphics::Index lightmapPosIndex = gLightmapBlitShader->GetAttribute("position");
	Graphics::Index lightmapTexIndex = gLightmapBlitShader->GetAttribute("texCoords");
	gLightmapMesh = gfx->CreateVertexLayout();
	gLightmapMesh->Set(lightmapPosIndex, *lightmapMesh, lightmapMeshPos);
	gLightmapMesh->Set(lightmapTexIndex, *lightmapMesh, lightmapMeshTex);
	gLightmapMesh->SetUserData(4);
	gBuffers[9] = lightmapMesh;

	/// Lit shader

	//PCMState* gPCMState;
	//PCMState* gLitNoPCM;
	//PCMState* gLitWithPCM;

	gLitNoPCM->shader = gfx->CreateShader(lit_vShader->text, lit_fShader->text);
	ReleaseText(lit_fShader);

	gLitWithPCM->shader = gfx->CreateShader(lit_vShader->text, lit_pcm_fShader->text);
	ReleaseText(lit_vShader);
	ReleaseText(lit_pcm_fShader);

	gLitNoPCM->modelIndex = gLitNoPCM->shader->GetUniform("model");
	gLitWithPCM->modelIndex = gLitWithPCM->shader->GetUniform("model");

	gLitNoPCM->shadowIndex = gLitNoPCM->shader->GetUniform("shadow");
	gLitWithPCM->shadowIndex = gLitWithPCM->shader->GetUniform("shadow");

	gLitNoPCM->viewIndex = gLitNoPCM->shader->GetUniform("view");
	gLitWithPCM->viewIndex = gLitWithPCM->shader->GetUniform("view");

	gLitNoPCM->projectionIndex = gLitNoPCM->shader->GetUniform("projection");
	gLitWithPCM->projectionIndex = gLitWithPCM->shader->GetUniform("projection");

	gLitNoPCM->albedoIndex = gLitNoPCM->shader->GetUniform("uColorSpec");
	gLitWithPCM->albedoIndex = gLitWithPCM->shader->GetUniform("uColorSpec");

	gLitNoPCM->lightmapIndex = gLitNoPCM->shader->GetUniform("uShadowMap");
	gLitWithPCM->lightmapIndex = gLitWithPCM->shader->GetUniform("uShadowMap");
	
	gLitNoPCM->normalIndex = gLitNoPCM->shader->GetUniform("uNormal");
	gLitWithPCM->normalIndex = gLitWithPCM->shader->GetUniform("uNormal");

	gLitNoPCM->lightDirection = gLitNoPCM->shader->GetUniform("LightDirection");
	gLitWithPCM->lightDirection = gLitWithPCM->shader->GetUniform("LightDirection");

	gLitNoPCM->lightColor = gLitNoPCM->shader->GetUniform("LightColor");
	gLitWithPCM->lightColor = gLitWithPCM->shader->GetUniform("LightColor");

	gLitNoPCM->viewPos = gLitNoPCM->shader->GetUniform("ViewPos");
	gLitWithPCM->viewPos = gLitWithPCM->shader->GetUniform("ViewPos");

	gLitNoPCM->ambientStrength = gLitNoPCM->shader->GetUniform("AmbientStrength");
	gLitWithPCM->ambientStrength = gLitWithPCM->shader->GetUniform("AmbientStrength");

	gLitNoPCM->ambientOnly = gLitNoPCM->shader->GetUniform("AmbientOnly");
	gLitWithPCM->ambientOnly = gLitWithPCM->shader->GetUniform("AmbientOnly");

	/// Initialize skull

	gHemiShader = gfx->CreateShader(hemi_vShader->text, hemi_fShader->text);
	ReleaseText(hemi_vShader);
	ReleaseText(hemi_fShader);
	
	Graphics::Buffer* positions = gfx->CreateBuffer(skullMesh->pos, sizeof(float) * 3 * skullMesh->numPos);
	Graphics::Buffer* texCoords = gfx->CreateBuffer(skullMesh->tex, sizeof(float) * 2 * skullMesh->numTex);
	Graphics::Buffer* normals = gfx->CreateBuffer(skullMesh->nrm, sizeof(float) * 3 * skullMesh->numNrm);
	Graphics::Buffer* tangents = gfx->CreateBuffer(skullMesh->tan, sizeof(float) * 3 * skullMesh->numTan);
	gBuffers[0] = positions;
	gBuffers[1] = texCoords;
	gBuffers[2] = normals;
	gBuffers[3] = tangents;

	Graphics::Index attribPos = gHemiShader->GetAttribute("aPos");
	Graphics::Index attribUv = gHemiShader->GetAttribute("aTexCoord");
	Graphics::Index attribNorm = gHemiShader->GetAttribute("aNorm");
	Graphics::Index attribTan = gHemiShader->GetAttribute("aTan");

	Graphics::BufferView posView(3, 0, Graphics::BufferType::Float32, 0);
	Graphics::BufferView normView(3, 0, Graphics::BufferType::Float32, 0);
	Graphics::BufferView tanView(3, 0, Graphics::BufferType::Float32, 0);
	Graphics::BufferView texView(2, 0, Graphics::BufferType::Float32, 0);

	gHemiSkullMesh = gfx->CreateVertexLayout();
	gHemiSkullMesh->Set(attribPos, *positions, posView);
	gHemiSkullMesh->Set(attribNorm, *normals, normView);
	gHemiSkullMesh->Set(attribTan, *tangents, tanView);
	gHemiSkullMesh->Set(attribUv, *texCoords, texView);
	gHemiSkullMesh->SetUserData(skullMesh->numPos);

	gLightmapSkullLayout->Set(lightmapPositionAttrib, *positions, posView);
	gLightmapSkullLayout->SetUserData(skullMesh->numPos);

	positions = gfx->CreateBuffer(planeMesh->pos, sizeof(float) * 3 * planeMesh->numPos);
	texCoords = gfx->CreateBuffer(planeMesh->tex, sizeof(float) * 2 * planeMesh->numTex);
	normals = gfx->CreateBuffer(planeMesh->nrm, sizeof(float) * 3 * planeMesh->numNrm);
	tangents = gfx->CreateBuffer(planeMesh->tan, sizeof(float) * 3 * planeMesh->numTan);

	gBuffers[4] = positions;
	gBuffers[5] = texCoords;
	gBuffers[6] = normals;
	gBuffers[7] = tangents;

	gLitNoPCM->planeMesh = gfx->CreateVertexLayout();
	attribPos = gLitNoPCM->shader->GetAttribute("aPos");
	attribUv = gLitNoPCM->shader->GetAttribute("aTexCoord");
	attribNorm = gLitNoPCM->shader->GetAttribute("aNorm");
	attribTan = gLitNoPCM->shader->GetAttribute("aTan");
	gLitNoPCM->planeMesh->Set(attribPos, *positions, posView);
	gLitNoPCM->planeMesh->Set(attribNorm, *normals, normView);
	gLitNoPCM->planeMesh->Set(attribTan, *tangents, tanView);
	gLitNoPCM->planeMesh->Set(attribUv, *texCoords, texView);
	gLitNoPCM->planeMesh->SetUserData(planeMesh->numPos);

	gLitWithPCM->planeMesh = gfx->CreateVertexLayout();
	attribPos = gLitWithPCM->shader->GetAttribute("aPos");
	attribUv = gLitWithPCM->shader->GetAttribute("aTexCoord");
	attribNorm = gLitWithPCM->shader->GetAttribute("aNorm");
	attribTan = gLitWithPCM->shader->GetAttribute("aTan");
	gLitWithPCM->planeMesh->Set(attribPos, *positions, posView);
	gLitWithPCM->planeMesh->Set(attribNorm, *normals, normView);
	gLitWithPCM->planeMesh->Set(attribTan, *tangents, tanView);
	gLitWithPCM->planeMesh->Set(attribUv, *texCoords, texView);
	gLitWithPCM->planeMesh->SetUserData(planeMesh->numPos);

	gLightmapPlaneLayout->Set(lightmapPositionAttrib, *positions, posView);
	gLightmapPlaneLayout->SetUserData(planeMesh->numPos);

	{ // Build composite mesh for comparison
		u32 bytesNeeded = skullMesh->numPos * sizeof(float) * 3 + skullMesh->numNrm * sizeof(float) * 3 + skullMesh->numTex * 2 * sizeof(float) + skullMesh->numTan * 3 * sizeof(float);
		float* float_arr = (float*)gfx->Allocate(bytesNeeded);
		u32 arr_size = 0;

		for (u32 i = 0; i < skullMesh->numPos; ++i) {
			float_arr[arr_size++] = skullMesh->pos[i * 3 + 0];
			float_arr[arr_size++] = skullMesh->pos[i * 3 + 1];
			float_arr[arr_size++] = skullMesh->pos[i * 3 + 2];

			float_arr[arr_size++] = skullMesh->nrm[i * 3 + 0];
			float_arr[arr_size++] = skullMesh->nrm[i * 3 + 1];
			float_arr[arr_size++] = skullMesh->nrm[i * 3 + 2];

			float_arr[arr_size++] = skullMesh->tan[i * 3 + 0];
			float_arr[arr_size++] = skullMesh->tan[i * 3 + 1];
			float_arr[arr_size++] = skullMesh->tan[i * 3 + 2];

			float_arr[arr_size++] = skullMesh->tex[i * 2 + 0];
			float_arr[arr_size++] = skullMesh->tex[i * 2 + 1];
		}

		posView = Graphics::BufferView(3, sizeof(float) * 11, Graphics::BufferType::Float32, 0);
		normView = Graphics::BufferView(3, sizeof(float) * 11, Graphics::BufferType::Float32, sizeof(float) * 3);
		tanView = Graphics::BufferView(3, sizeof(float) * 11, Graphics::BufferType::Float32, sizeof(float) * 6);
		texView = Graphics::BufferView(2, sizeof(float) * 11, Graphics::BufferType::Float32, sizeof(float) * 9);

		gLitNoPCM->skullMesh = gfx->CreateVertexLayout();
		attribPos = gLitNoPCM->shader->GetAttribute("aPos");
		attribUv = gLitNoPCM->shader->GetAttribute("aTexCoord");
		attribNorm = gLitNoPCM->shader->GetAttribute("aNorm");
		attribTan = gLitNoPCM->shader->GetAttribute("aTan");
		Graphics::Buffer* compositeBuff = gfx->CreateBuffer(float_arr, arr_size * sizeof(float));
		gLitNoPCM->skullMesh->Set(attribPos, *compositeBuff, posView);
		gLitNoPCM->skullMesh->Set(attribNorm, *compositeBuff, normView);
		gLitNoPCM->skullMesh->Set(attribTan, *compositeBuff, tanView);
		gLitNoPCM->skullMesh->Set(attribUv, *compositeBuff, texView);
		gLitNoPCM->skullMesh->SetUserData(skullMesh->numPos);
		gBuffers[8] = compositeBuff;

		gLitWithPCM->skullMesh = gfx->CreateVertexLayout();
		attribPos = gLitWithPCM->shader->GetAttribute("aPos");
		attribUv = gLitWithPCM->shader->GetAttribute("aTexCoord");
		attribNorm = gLitWithPCM->shader->GetAttribute("aNorm");
		attribTan = gLitWithPCM->shader->GetAttribute("aTan");
		gLitWithPCM->skullMesh->Set(attribPos, *compositeBuff, posView);
		gLitWithPCM->skullMesh->Set(attribNorm, *compositeBuff, normView);
		gLitWithPCM->skullMesh->Set(attribTan, *compositeBuff, tanView);
		gLitWithPCM->skullMesh->Set(attribUv, *compositeBuff, texView);
		gLitWithPCM->skullMesh->SetUserData(skullMesh->numPos);
		gBuffers[8] = compositeBuff;

		gfx->Release(float_arr);
	}
	ReleaseMesh(skullMesh);
	ReleaseMesh(planeMesh);

	gAlbedoIndex = gHemiShader->GetUniform("uColorSpec");
	gNormalIndex = gHemiShader->GetUniform("uNormal");
	gModelIndex = gHemiShader->GetUniform("model");
	gViewIndex = gHemiShader->GetUniform("view");
	gHemiTop = gHemiShader->GetUniform("HemiTop");
	gHemiBottom = gHemiShader->GetUniform("HemiBottom");
	gProjectionIndex = gHemiShader->GetUniform("projection");

	gLightDirection = gHemiShader->GetUniform("LightDirection");
	gLightColor = gHemiShader->GetUniform("LightColor");
	gLightAmbientOnly = gHemiShader->GetUniform("AmbientOnly");
	gViewPos = gHemiShader->GetUniform("ViewPos");

	Graphics::TextureFormat format = Graphics::TextureFormat::RGBA8;
	if (skullAlbedo->channels == 3) {
		format = Graphics::TextureFormat::RGB8;
	}
	gSkullTextureAlbedo = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, skullAlbedo->width, skullAlbedo->height, skullAlbedo->data, format,true);
	ReleaseTexture(skullAlbedo);

	format = Graphics::TextureFormat::RGBA8;
	if (skullNormal->channels == 3) {
		format = Graphics::TextureFormat::RGB8;
	}
	gSkullTextureNormal = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, skullNormal->width, skullNormal->height, skullNormal->data, format, true);
	ReleaseTexture(skullNormal);
	
	format = Graphics::TextureFormat::RGBA8;
	if (planeAlbedo->channels == 3) {
		format = Graphics::TextureFormat::RGB8;
	}
	gPlaneTextureAlbedo = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, planeAlbedo->width, planeAlbedo->height, planeAlbedo->data, format, true);
	ReleaseTexture(planeAlbedo);
	
	
	format = Graphics::TextureFormat::RGBA8;
	if (planeNormal->channels == 3) {
		format = Graphics::TextureFormat::RGB8;
	}
	gPlaneTextureNormal = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, planeNormal->width, planeNormal->height, planeNormal->data, format, true);
	ReleaseTexture(planeNormal);

	gfx->SetDepthState(true);

	gLightmapMVP = gLightmapDrawShader->GetUniform("mvp");
	GraphicsAssert(gLightmapMVP.valid, "INvalid lightmap mvp?");

	isFinishedInitializing = true;
}

void Update(Graphics::Device* g, float deltaTime) {
	if (!IsRunning) {
		return;
	}

	if (numFilesToLoad != 0) {
		return;
	}

	if (!isFinishedInitializing) {
		FinishInitializing(g);
		isFinishedInitializing = true;
		GraphicsAssert(gLightmapMVP.valid, "(3) INvalid lightmap mvp?");
		return;
	}
	GraphicsAssert(gLightmapMVP.valid, "(4) INvalid lightmap mvp?");

	if (lastPCM != enablePCM) {
		gLightmapFBO->AttachDepth(*gLightmapDepth, enablePCM);
		if (enablePCM) {
			gPCMState = gLitWithPCM;
		}
		else {
			gPCMState = gLitNoPCM;
		}
		lastPCM = enablePCM;
	}

#if 1
	camTime += deltaTime * 0.25f;
	while (camTime >= 360.0f) {
		camTime -= 360.0f;
	}

	lightTime += deltaTime * lightDir;
	if (lightTime > 5.0f) {
		lightDir *= -1.0f;
		lightTime = 5.0f;
	}
	else if (lightTime < 0.0f) {
		lightDir *= -1.0f;
		lightTime = 0.0f;
	}
#else
	camTime = 25;
#endif
}

void Render(Graphics::Device * gfx, int x, int y, int w, int h) {
	if (!IsRunning) {
		return;
	}
	if (!isFinishedInitializing) {
		return;
	}

	GraphicsAssert(gLightmapMVP.valid, "(5) INvalid lightmap mvp?");

	float camX = FastSin(camTime) * cameraRadius;
	float camZ = FastCos(camTime) * cameraRadius;
	vec3 cameraPos = vec3(camX, cameraHeight, camZ);
	mat4 view = lookAt(cameraPos, cameraTarget, vec3(0, 1, 0));
	mat4 projection = perspective(45.0f, 800.0f / 600.0f, 0.1f, 1000.0f);
	mat4 model1 = mat4(0.1f, 0.0, 0.0, 0.0,
		0.0, 0.1f, 0.0, 0.0,
		0.0, 0.0, 0.1f, 0.0,
		1.5f, 0.0f, -0.5, 1.0f);
	mat4 model2 = mat4(0.1f, 0.0, 0.0, 0.0,
		0.0, 0.1f, 0.0, 0.0,
		0.0, 0.0, 0.1f, 0.0,
		-0.5f, -1.0f, 1.5, 1.0f);
	mat4 model3 = mat4(2.0f, 0.0, 0.0, 0.0,
		0.0, 2.0f, 0.0, 0.0,
		0.0, 0.0, 2.0f, 0.0,
		0.0f, -1.5, 0.0, 1.0f);
	vec3 lightDir = vec3(0.2, -1.0, -1);
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	float ambient = 0.2f;
	vec3 HemiTop = vec3(0.3f, 0.3f, 0.3f);
	vec3 HemiBottom = vec3(ambient, ambient, ambient);
	Graphics::Sampler sampler;

	Graphics::Sampler depthSampler(Graphics::WrapMode::Clamp, Graphics::WrapMode::Clamp, Graphics::Filter::Nearest, Graphics::Filter::Nearest, Graphics::Filter::Nearest);
	if (gPCMState == gLitWithPCM) {
		depthSampler = Graphics::Sampler(Graphics::WrapMode::Clamp, Graphics::WrapMode::Clamp, Graphics::Filter::Linear, Graphics::Filter::Linear, Graphics::Filter::Linear);
	}

	vec3 lightCameraPosition = cameraTarget - normalized(lightDir) * 10.0f;
	mat4 ShadowView = lookAt(lightCameraPosition, cameraTarget, vec3(0, 1, 0));
	mat4 ShadowProjection = ortho(-7.5, 7.5, -7.5, 7.5, 0.01, 20.0);

	mat4 shadowMapAdjustment(
	0.5f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.5f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.5f - 0.01f, 1.0f);

	{ // Draw lightmap
		gfx->SetRenderTarget(gLightmapFBO);
		gfx->SetViewport(0, 0, SHADOWMAP_RES, SHADOWMAP_RES);
		gfx->Clear(1.0f);

		gfx->SetFaceCulling(Graphics::CullFace::Front); // TODO: This seems wrong... Should probably change name from set visibility to SetCullFAce.
#if 1
		gfx->Bind(gLightmapDrawShader);
		mat4 mvp = ShadowProjection * ShadowView * model1;
		GraphicsAssert(gLightmapMVP.valid, "(2) INvalid lightmap mvp?");
		gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
		GraphicsAssert(gLightmapMVP.valid, "(a) INvalid lightmap mvp?");
		gfx->Draw(*gLightmapSkullLayout, Graphics::DrawMode::Triangles, 0, gLightmapSkullLayout->GetUserData());

		mvp = ShadowProjection* ShadowView* model2;
		GraphicsAssert(gLightmapMVP.valid, "(b) INvalid lightmap mvp?");
		gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
		GraphicsAssert(gLightmapMVP.valid, "(c) INvalid lightmap mvp?");
		gfx->Draw(*gLightmapSkullLayout, Graphics::DrawMode::Triangles, 0, gLightmapSkullLayout->GetUserData());

		mvp = ShadowProjection * ShadowView * model3;
		GraphicsAssert(gLightmapMVP.valid, "(d) INvalid lightmap mvp?");
		gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
		GraphicsAssert(gLightmapMVP.valid, "(e INvalid lightmap mvp?");
		//gfx->Draw(*gLightmapPlaneLayout, Graphics::DrawMode::Triangles, 0, gLightmapPlaneLayout->GetUserData());
#endif
		gfx->SetFaceCulling(Graphics::CullFace::Back);
	}

	mat4 shadowMatrix1 = shadowMapAdjustment * ShadowProjection * ShadowView * model1;
	mat4 shadowMatrix2 = shadowMapAdjustment * ShadowProjection * ShadowView * model2;
	mat4 shadowMatrix3 = shadowMapAdjustment * ShadowProjection * ShadowView * model3;


	{ // Draw scene
		gfx->SetRenderTarget(0);
		gfx->SetViewport(0, 0, 800, 600);
		gfx->Clear(0.4, 0.5, 0.6, 1.0);
		gfx->Bind(gHemiShader);

		gfx->Bind(gLightDirection, Graphics::UniformType::Float3, lightDir.v);
		gfx->Bind(gLightColor, Graphics::UniformType::Float3, lightColor.v);
		gfx->Bind(gLightAmbientOnly, Graphics::UniformType::Float1, &ambientOnly);
		gfx->Bind(gViewPos, Graphics::UniformType::Float3, cameraPos.v);

		gfx->Bind(gAlbedoIndex, *gSkullTextureAlbedo, sampler);
		gfx->Bind(gNormalIndex, *gSkullTextureNormal, sampler);
		gfx->Bind(gModelIndex, Graphics::UniformType::Float16, model1.v);
		gfx->Bind(gViewIndex, Graphics::UniformType::Float16, view.v);
		gfx->Bind(gHemiTop, Graphics::UniformType::Float3, HemiTop.v);
		gfx->Bind(gHemiBottom, Graphics::UniformType::Float3, HemiBottom.v);
		gfx->Bind(gProjectionIndex, Graphics::UniformType::Float16, projection.v);
		gfx->Draw(*gHemiSkullMesh, Graphics::DrawMode::Triangles, 0, gHemiSkullMesh->GetUserData());

		gfx->Bind(gPCMState->shader);

		gfx->Bind(gPCMState->modelIndex, Graphics::UniformType::Float16, model2.v);
		gfx->Bind(gPCMState->shadowIndex, Graphics::UniformType::Float16, shadowMatrix2.v);
		gfx->Bind(gPCMState->viewIndex, Graphics::UniformType::Float16, view.v);
		gfx->Bind(gPCMState->projectionIndex, Graphics::UniformType::Float16, projection.v);

		gfx->Bind(gPCMState->normalIndex, *gSkullTextureNormal, sampler);
		gfx->Bind(gPCMState->albedoIndex, *gSkullTextureAlbedo, sampler);
		GraphicsAssert(gPCMState->normalIndex.id != gPCMState->albedoIndex.id, "Bad ID's");
		gfx->Bind(gPCMState->lightmapIndex, *gLightmapDepth, depthSampler);
		gfx->Bind(gPCMState->lightDirection, Graphics::UniformType::Float3, lightDir.v);
		gfx->Bind(gPCMState->lightColor, Graphics::UniformType::Float3, lightColor.v);
		gfx->Bind(gPCMState->viewPos, Graphics::UniformType::Float3, cameraPos.v);
		gfx->Bind(gPCMState->ambientStrength, Graphics::UniformType::Float1, &ambient);
		gfx->Bind(gPCMState->ambientOnly, Graphics::UniformType::Float1, &ambientOnly);
		
		gfx->Draw(*gPCMState->skullMesh, Graphics::DrawMode::Triangles, 0, gPCMState->skullMesh->GetUserData());

		ambient = 0.0f;
		gfx->Bind(gPCMState->modelIndex, Graphics::UniformType::Float16, model3.v);
		gfx->Bind(gPCMState->shadowIndex, Graphics::UniformType::Float16, shadowMatrix3.v);
		gfx->Bind(gPCMState->albedoIndex, *gPlaneTextureAlbedo, sampler);
		gfx->Bind(gPCMState->lightmapIndex, *gLightmapDepth, depthSampler);
		gfx->Bind(gPCMState->normalIndex, *gPlaneTextureNormal, sampler);
		gfx->Bind(gPCMState->ambientStrength, Graphics::UniformType::Float1, &ambient);
		gfx->Draw(*gPCMState->planeMesh, Graphics::DrawMode::Triangles, 0, gPCMState->planeMesh->GetUserData());
	}

	if (ShowDepth) { // Draw debug
		gfx->SetViewport(w - 20 - 256, h - 20 - 256, 266, 266);
		gfx->SetScissorState(true, w - 20 - 256, h - 20 - 256, 266, 266);
		gfx->Clear(0, 0, 0, 1);
		gfx->SetScissorState(false);
		gfx->SetViewport(w - 10 - 5 - 256, h - 10 - 5 - 256, 256, 256);

		gfx->Bind(gLightmapBlitShader);
		//gfx->Bind(gLightmapFboAttachment, *gLightmapColor, sampler);
		gfx->Bind(gLightmapFboAttachment, *gLightmapDepth, depthSampler);
		gfx->Draw(*gLightmapMesh, Graphics::DrawMode::TriangleStrip, 0, gLightmapMesh->GetUserData());
		gfx->SetViewport(0, 0, 800, 600);
	}
}

void Shutdown(Graphics::Device* gfx) {
	IsRunning = false;

	gfx->SetRenderTarget(0);
	gfx->Bind(0);

	for (u32 i = 0; i < NUM_BUFFERS; ++i) {
		gfx->Destroy(gBuffers[i]);
	}

	gfx->Destroy(gLightmapPlaneLayout);
	gfx->Destroy(gLightmapSkullLayout);
	gfx->Destroy(gLightmapMesh);
	gfx->Destroy(gLightmapFBO);
	gfx->Destroy(gLightmapColor);
	gfx->Destroy(gLightmapDepth);
	gfx->Destroy(gLightmapBlitShader);
	gfx->Destroy(gLightmapDrawShader);
	gfx->Destroy(gLitNoPCM->shader);
	gfx->Destroy(gLitWithPCM->shader);
	gfx->Destroy(gLitNoPCM->skullMesh);
	gfx->Destroy(gLitWithPCM->skullMesh);
	gfx->Destroy(gLitNoPCM->planeMesh);
	gfx->Destroy(gLitWithPCM->planeMesh);
	gfx->Destroy(gPlaneTextureAlbedo);
	gfx->Destroy(gPlaneTextureNormal);
	gfx->Destroy(gHemiShader);
	gfx->Destroy(gHemiSkullMesh);
	gfx->Destroy(gSkullTextureAlbedo);
	gfx->Destroy(gSkullTextureNormal);

	gfx->Release(gLitNoPCM);
	gfx->Release(gLitWithPCM);

	Graphics::Shutdown(*gfx);
	gfx = 0;
}