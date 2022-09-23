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

MeshFile* LoadMesh(const char* path); 
TextureFile* LoadTexture(const char* path);
TextFile* LoadText(const char* path);

void ReleaseMesh(MeshFile* file);
void ReleaseTexture(TextureFile* file);
void ReleaseText(TextFile* file);