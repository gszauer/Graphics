#pragma once

struct TextureFile {
	unsigned int width;
	unsigned int height;
	unsigned int channels;
	void* data;
};

struct MeshFile {
	unsigned int numPos;
	unsigned int numNrm;
	unsigned int numTex;
	unsigned int numTan;
	float* pos;
	float* nrm;
	float* tex;
	float* tan;
};

struct TextFile {
	char* text;
	unsigned int length;
};

typedef void (*OnMeshLoaded)(const char* path, MeshFile* file);
typedef void (*OnTextureLoaded)(const char* path, TextureFile* file);
typedef void (*OnTextFileLoaded)(const char* path, TextFile* file);

void LoadMesh(const char* path, OnMeshLoaded onMeshLoad);
void LoadTexture(const char* path, OnTextureLoaded onTextureLoad);
void LoadText(const char* path, OnTextFileLoaded onTextLoad);

void ReleaseMesh(MeshFile* file);
void ReleaseTexture(TextureFile* file);
void ReleaseText(TextFile* file);