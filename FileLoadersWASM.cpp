#include "FileLoaders.h"
#include "math.h"

namespace Internal {
	void CalculateTangentArray(unsigned int vertexCount, vec3* vertex, vec3* normal,
		vec2* texcoord, vec3* outTangent)
	{
		vec3* tan1 = (vec3*)wasmGraphics_AllocateMem(vertexCount * 2 * sizeof(vec3));
		vec3* tan2 = tan1 + vertexCount;

		for (long a = 0; a < vertexCount; a += 3)
		{
			vec3 v1 = vertex[a + 0];
			vec3 v2 = vertex[a + 1];
			vec3 v3 = vertex[a + 2];

			vec2 w1 = texcoord[a + 0];
			vec2 w2 = texcoord[a + 1];
			vec2 w3 = texcoord[a + 2];

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0f / (s1 * t2 - s2 * t1);
			vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
			vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);

			tan1[a + 0] = tan1[a + 0] + sdir;
			tan1[a + 1] = tan1[a + 1] + sdir;
			tan1[a + 2] = tan1[a + 2] + sdir;

			tan2[a + 0] = tan2[a + 0] + tdir;
			tan2[a + 1] = tan2[a + 1] + tdir;
			tan2[a + 2] = tan2[a + 2] + tdir;
		}

		for (long a = 0; a < vertexCount; a++) {
			vec3 n = normal[a];
			vec3 t = tan1[a];

			// Gram-Schmidt orthogonalize
			outTangent[a] = normalized(t - n * dot(n, t));

			// Calculate handedness
			//tangent[a].w = (dot(cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
		}

		//wasmGraphics_ReleaseMem(tan1);
	}

	u32 StrLen(const char* str) {
		if (str == 0) {
			return 0;
		}

		const char *s = str;
		while (*s) {
			++s;
		}
		return (s - str);
	}
}

export void FinishLoadingText(const char* path, OnTextFileLoaded triggerThisCallback, void* withThisData, u32 whichHasThisManyBytes) {
	TextFile* result = (TextFile*)wasmGraphics_AllocateMem(sizeof(TextFile));
	result->length = whichHasThisManyBytes;
	result->text = (char*)withThisData;
	triggerThisCallback(path, result);
}

extern "C" void wasmFileLoaderLoadText(const char* path, int len, OnTextFileLoaded callback);

void LoadText(const char* path, OnTextFileLoaded onTextLoad) {
	u32 path_len = Internal::StrLen(path);

	// Here we pass the path to load, and the callback to eventually call back to js.
	// js will stash that pointer, and load the file. When the file is loaded, 
	// FinishLoadingText will be called. onTextLoad will be it's first argument,
	// It will allocate the proper data to turn the rest of its arguments into a text file.
	
	wasmFileLoaderLoadText(path, path_len, onTextLoad);
}

void ReleaseText(TextFile* file) {
	wasmGraphics_ReleaseMem(file->text);
	wasmGraphics_ReleaseMem(file);
}

export void FinishLoadingMesh(const char* path, OnMeshLoaded triggerThisCallback, void* withThisData, u32 whichHasThisManyBytes) {
	unsigned int* uint_data = (unsigned int*)withThisData;
	unsigned int sizes[3];
	sizes[0] = uint_data[0];
	sizes[1] = uint_data[1];
	sizes[2] = uint_data[2];

	unsigned int mem_needed = sizeof(MeshFile);
	void* mem = wasmGraphics_AllocateMem(mem_needed + 1);
	unsigned char* iter = (unsigned char* )mem;
	
	MeshFile* result = (MeshFile*)iter;
	iter += sizeof(MeshFile);

	float* pos = (float*)(uint_data + 3);
	float* nrm = pos + sizes[0] * 3;
	float* tex = nrm + sizes[1] * 3;

	result->tan = tex + sizes[2] * 2;
	Internal::CalculateTangentArray(sizes[0], (vec3*)pos, (vec3*)nrm, (vec2*)tex, (vec3*)result->tan);

	result->numTan = sizes[0];
	result->numPos = sizes[0];
	result->numNrm = sizes[1];
	result->numTex = sizes[2];
	result->pos = pos;
	result->nrm = nrm;
	result->tex = tex;

	triggerThisCallback(path, result);
}

extern "C" void wasmFileLoaderLoadMesh(const char* path, int len, OnMeshLoaded callback);

void LoadMesh(const char* path, OnMeshLoaded onMeshLoad) {
	u32 path_len = Internal::StrLen(path);

	wasmFileLoaderLoadMesh(path, path_len, onMeshLoad);
}

void ReleaseMesh(MeshFile* file) {
	void* dataPtr = file->pos - 3;
	//wasmGraphics_ReleaseMem(dataPtr);
	//wasmGraphics_ReleaseMem(file);
}

extern "C" void wasmFileLoaderLoadTexture(const char* path, int len, OnTextureLoaded callback);

export void FinishLoadingTexture(const char* path, OnTextureLoaded triggerThisCallback, void* withThisData, u32 whichHasThisManyBytes) {
	unsigned int* uint_data = (unsigned int*)withThisData;
	unsigned int sizes[3];
	sizes[0] = uint_data[0];
	sizes[1] = uint_data[1];
	sizes[2] = uint_data[2];

	unsigned int mem_needed = sizeof(TextureFile);
	void* mem = wasmGraphics_AllocateMem(mem_needed);
	unsigned char* iter = (unsigned char*)mem;

	TextureFile* result = (TextureFile*)iter;
	iter += sizeof(TextureFile);

	result->width = sizes[0];
	result->height = sizes[1];
	result->channels = sizes[2];
	result->data = (unsigned char*)(uint_data + 3);

	triggerThisCallback(path, result);
}

void LoadTexture(const char* path, OnTextureLoaded onTextureLoad) {
	u32 path_len = Internal::StrLen(path);

	wasmFileLoaderLoadTexture(path, path_len, onTextureLoad);
}

void ReleaseTexture(TextureFile* file) {
	void* dataPtr = (unsigned char*)file->data - 3;
	//wasmGraphics_ReleaseMem(dataPtr);
	//wasmGraphics_ReleaseMem(file);
}

