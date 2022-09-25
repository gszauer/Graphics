#include "Graphics.h"
#include "math.h"
#include "FileLoaders.h"

#define SHADOWMAP_RES 512

Graphics::FrameBuffer*  gLightmapFBO;
Graphics::Texture*		gLightmapDepth;
Graphics::Texture*		gLightmapColor;

Graphics::Shader*		gLightmapDrawShader;
Graphics::Shader*		gLightmapBlitShader;
Graphics::Index			gLightmapFboAttachment;
Graphics::VertexLayout*	gLightmapMesh;
Graphics::VertexLayout* gLightmapSkullLayout;
Graphics::VertexLayout* gLightmapPlaneLayout;
Graphics::Index			gLightmapMVP;

Graphics::Shader* gLitShader;
Graphics::Shader* gLitShaderPCM;
Graphics::VertexLayout* gLitSkullMesh;
Graphics::VertexLayout* gLitPlaneMesh;
Graphics::Texture* gPlaneTextureAlbedo;
Graphics::Texture* gPlaneTextureNormal;
Graphics::Index gLitModelIndex;
Graphics::Index gLitShadowIndex;
Graphics::Index gLitViewIndex;
Graphics::Index gLitProjectionIndex;
Graphics::Index gLitAlbedoIndex;
Graphics::Index gLitLightmapIndex;
Graphics::Index gLitNormalIndex;
Graphics::Index gLitLightDirection;
Graphics::Index gLitLightColor;
Graphics::Index gLitViewPos;
Graphics::Index gLitAmbientStrength;
Graphics::Index gLitAmbientOnly;

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
Graphics::Shader* gPCMShader;

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

void Initialize(Graphics::Dependencies* platform, Graphics::Device* gfx) {
	IsRunning = true;
	isFinishedInitializing = false;
	enablePCM = true;
	lastPCM = true;
	ambientOnly = 0.0f;

	cameraRadius = 7.0f;
	cameraHeight = 3.0f;
	camTime = lightTime = 0.0f;
	lightDir = 1.0f;
	cameraTarget.y = 0.0f;

	globalDevice = gfx;

	gLightmapFBO = gfx->CreateFrameBuffer();
	gLightmapColor = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, SHADOWMAP_RES, SHADOWMAP_RES);
	gLightmapDepth = gfx->CreateTexture(Graphics::TextureFormat::Depth, SHADOWMAP_RES, SHADOWMAP_RES, 0, Graphics::TextureFormat::Depth, false);
	//gLightmapFBO->AttachColor(*gLightmapColor);
	gLightmapFBO->AttachDepth(*gLightmapDepth);
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
	LoadTexture("assets/Skull_Normal.texture", [](const char* path, TextureFile* file) {
		skullNormal = file;
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
	LoadTexture("assets/Skull_AlbedoSpec.texture", [](const char* path, TextureFile* file) {
		skullAlbedo = file;
		numFilesToLoad -= 1;
		
		});
}

void FinishInitializing(Graphics::Device* gfx) {
	gLightmapBlitShader = gfx->CreateShader(blit_depth_vShader->text, blit_depth_fShader->text);
	ReleaseText(blit_depth_vShader);
	ReleaseText(blit_depth_fShader);
	gLightmapFboAttachment = gLightmapBlitShader->GetUniform("fboAttachment");
	
	gLightmapDrawShader = gfx->CreateShader(lightmap_vShader->text, lightmap_fShader->text);
	ReleaseText(lightmap_vShader);
	ReleaseText(lightmap_fShader);

	gLightmapSkullLayout = gfx->CreateVertexLayout();
	Graphics::Index lightmapPositionAttrib = gLightmapDrawShader->GetAttribute("position");
	gLightmapMVP = gLightmapDrawShader->GetUniform("mvp");

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
	
	gLitShader = gfx->CreateShader(lit_vShader->text, lit_fShader->text);
	ReleaseText(lit_fShader);

	gLitShaderPCM = gfx->CreateShader(lit_vShader->text, lit_pcm_fShader->text);
	gPCMShader = gLitShaderPCM;
	ReleaseText(lit_vShader);
	ReleaseText(lit_pcm_fShader);

	gLitModelIndex = gLitShader->GetUniform("model");
	gLitShadowIndex = gLitShader->GetUniform("shadow");
	gLitViewIndex = gLitShader->GetUniform("view");
	gLitProjectionIndex = gLitShader->GetUniform("projection");
	gLitAlbedoIndex = gLitShader->GetUniform("uColorSpec");
	gLitLightmapIndex = gLitShader->GetUniform("uShadowMap");
	
	gLitNormalIndex = gLitShader->GetUniform("uNormal");
	gLitLightDirection = gLitShader->GetUniform("LightDirection");
	gLitLightColor = gLitShader->GetUniform("LightColor");
	gLitViewPos = gLitShader->GetUniform("ViewPos");
	gLitAmbientStrength = gLitShader->GetUniform("AmbientStrength");
	gLitAmbientOnly = gLitShader->GetUniform("AmbientOnly");

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

	gLitPlaneMesh = gfx->CreateVertexLayout();
	gLitPlaneMesh->Set(attribPos, *positions, posView);
	gLitPlaneMesh->Set(attribNorm, *normals, normView);
	gLitPlaneMesh->Set(attribTan, *tangents, tanView);
	gLitPlaneMesh->Set(attribUv, *texCoords, texView);
	gLitPlaneMesh->SetUserData(planeMesh->numPos);

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

		attribPos = gLitShader->GetAttribute("aPos");
		attribUv = gLitShader->GetAttribute("aTexCoord");
		attribNorm = gLitShader->GetAttribute("aNorm");
		attribTan = gLitShader->GetAttribute("aTan");

		gLitSkullMesh = gfx->CreateVertexLayout();
		Graphics::Buffer* compositeBuff = gfx->CreateBuffer(float_arr, arr_size * sizeof(float));
		gLitSkullMesh->Set(attribPos, *compositeBuff, posView);
		gLitSkullMesh->Set(attribNorm, *compositeBuff, normView);
		gLitSkullMesh->Set(attribTan, *compositeBuff, tanView);
		gLitSkullMesh->Set(attribUv, *compositeBuff, texView);
		gLitSkullMesh->SetUserData(skullMesh->numPos);
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
		GraphicsAssert(false, "Finished Initializing");
		return;
	}

	return;

	if (lastPCM != enablePCM) {
		gLightmapDepth->SetPCM(enablePCM);
		if (enablePCM) {
			gPCMShader = gLitShaderPCM;
		}
		else {
			gPCMShader = gLitShader;
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
	Graphics::Sampler depthSampler;

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
		gfx->Clear(1.0f, 1.0f, 1.0f, 1.0f);

		gfx->Bind(gLightmapDrawShader);
		mat4 mvp = ShadowProjection * ShadowView * model1;
		gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
		gfx->Draw(*gLightmapSkullLayout, Graphics::DrawMode::Triangles, 0, gLightmapSkullLayout->GetUserData());

		mvp = ShadowProjection* ShadowView* model2;
		gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
		gfx->Draw(*gLightmapSkullLayout, Graphics::DrawMode::Triangles, 0, gLightmapSkullLayout->GetUserData());

		mvp = ShadowProjection * ShadowView * model3;
		gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
		//gfx->Draw(*gLightmapPlaneLayout, Graphics::DrawMode::Triangles, 0, gLightmapPlaneLayout->GetUserData());

		gfx->SetRenderTarget(0);
		gfx->SetViewport(0, 0, 800, 600);
	}

	mat4 shadowMatrix1 = shadowMapAdjustment * ShadowProjection * ShadowView * model1;
	mat4 shadowMatrix2 = shadowMapAdjustment * ShadowProjection * ShadowView * model2;
	mat4 shadowMatrix3 = shadowMapAdjustment * ShadowProjection * ShadowView * model3;


	{ // Draw scene
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

		gfx->Bind(gPCMShader);

		gfx->Bind(gLitModelIndex, Graphics::UniformType::Float16, model2.v);
		gfx->Bind(gLitShadowIndex, Graphics::UniformType::Float16, shadowMatrix2.v);
		gfx->Bind(gLitViewIndex, Graphics::UniformType::Float16, view.v);
		gfx->Bind(gLitProjectionIndex, Graphics::UniformType::Float16, projection.v);

		gfx->Bind(gLitAlbedoIndex, *gSkullTextureAlbedo, sampler);
		gfx->Bind(gLitLightmapIndex, *gLightmapDepth, depthSampler);
		gfx->Bind(gLitNormalIndex, *gSkullTextureNormal, sampler);
		gfx->Bind(gLitLightDirection, Graphics::UniformType::Float3, lightDir.v);
		gfx->Bind(gLitLightColor, Graphics::UniformType::Float3, lightColor.v);
		gfx->Bind(gLitViewPos, Graphics::UniformType::Float3, cameraPos.v);
		gfx->Bind(gLitAmbientStrength, Graphics::UniformType::Float1, &ambient);
		gfx->Bind(gLitAmbientOnly, Graphics::UniformType::Float1, &ambientOnly);

		
		gfx->Draw(*gLitSkullMesh, Graphics::DrawMode::Triangles, 0, gLitSkullMesh->GetUserData());

		ambient = 0.0f;
		gfx->Bind(gLitModelIndex, Graphics::UniformType::Float16, model3.v);
		gfx->Bind(gLitShadowIndex, Graphics::UniformType::Float16, shadowMatrix3.v);
		gfx->Bind(gLitAlbedoIndex, *gPlaneTextureAlbedo, sampler);
		gfx->Bind(gLitLightmapIndex, *gLightmapDepth, depthSampler);
		gfx->Bind(gLitNormalIndex, *gPlaneTextureNormal, sampler);
		gfx->Bind(gLitAmbientStrength, Graphics::UniformType::Float1, &ambient);
		gfx->Draw(*gLitPlaneMesh, Graphics::DrawMode::Triangles, 0, gLitPlaneMesh->GetUserData());
	}

	if (ShowDepth) { // Draw debug
		gfx->SetViewport(w - 20 - 256, h - 20 - 256, 266, 266);
		gfx->SetScissorState(true, w - 20 - 256, h - 20 - 256, 266, 266);
		gfx->Clear(0, 0, 0, 1);
		gfx->SetScissorState(false);
		gfx->SetViewport(w - 10 - 5 - 256, h - 10 - 5 - 256, 256, 256);

		gfx->Bind(gLightmapBlitShader);
		//gfx->Bind(gLightmapFboAttachment, *gLightmapColor, sampler);
		gfx->Bind(gLightmapFboAttachment, *gLightmapDepth, sampler);
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
	gfx->Destroy(gLitShader);
	gfx->Destroy(gLitShaderPCM);
	gfx->Destroy(gLitSkullMesh);
	gfx->Destroy(gLitPlaneMesh);
	gfx->Destroy(gPlaneTextureAlbedo);
	gfx->Destroy(gPlaneTextureNormal);
	gfx->Destroy(gHemiShader);
	gfx->Destroy(gHemiSkullMesh);
	gfx->Destroy(gSkullTextureAlbedo);
	gfx->Destroy(gSkullTextureNormal);

	Graphics::Shutdown(*gfx);
	gfx = 0;
}