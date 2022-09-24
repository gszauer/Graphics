#include "FileLoaders.h"
#include "math.h"
#include <windows.h>

namespace Internal {
	void CalculateTangentArray(unsigned int vertexCount, vec3* vertex, vec3* normal,
		vec2* texcoord, vec3* outTangent)
	{
		vec3* tan1 = (vec3*)VirtualAlloc(0, vertexCount * 2 * sizeof(vec3), MEM_COMMIT, PAGE_READWRITE);
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

		VirtualFree(tan1, 0, MEM_RELEASE);
	}
}


void LoadText(const char* path, OnTextFileLoaded onTextLoad) {
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD bytesInFile = GetFileSize(hFile, 0);
	DWORD bytesRead = 0;

	unsigned int mem_needed = sizeof(TextFile) + bytesRead;
	void* mem = VirtualAlloc(0, mem_needed + 1, MEM_COMMIT, PAGE_READWRITE);
	unsigned char* iter = (unsigned char*)mem;
	TextFile* result = (TextFile*)iter;
	iter += sizeof(TextFile);

	if (hFile == INVALID_HANDLE_VALUE) {
		onTextLoad(path, 0);
		return;
	}
	if (ReadFile(hFile, iter, bytesInFile, &bytesRead, NULL) == 0) {
		CloseHandle(hFile);
		onTextLoad(path, 0);
		return;
	}

	result->length = (unsigned int)bytesInFile;
	result->text = (char*)iter;
	CloseHandle(hFile);
	onTextLoad(path, result);
}

void LoadMesh(const char* path, OnMeshLoaded onMeshLoad) {
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD bytesRead = 0;

	unsigned int sizes[3];

	if (hFile == INVALID_HANDLE_VALUE) {
		onMeshLoad(path, 0);
		return;
	}
	if (ReadFile(hFile, sizes, sizeof(unsigned int) * 3, &bytesRead, NULL) == 0) {
		CloseHandle(hFile);
		onMeshLoad(path, 0);
		return;
	}

	unsigned int mem_needed = sizeof(MeshFile) + sizeof(float) * 3 * sizes[0] * 2 + sizeof(float) * 3 * sizes[1] + sizeof(float) * 2 * sizes[2];
	void* mem = VirtualAlloc(0, mem_needed + 1, MEM_COMMIT, PAGE_READWRITE);
	unsigned char* iter = (unsigned char* )mem;
	
	MeshFile* result = (MeshFile*)iter;
	iter += sizeof(MeshFile);

	float* pos = (float*)iter;
	if (sizes[0] != 0) {
		if (ReadFile(hFile, pos, sizeof(float) * 3 * sizes[0], &bytesRead, NULL) == 0) {
			CloseHandle(hFile);
			onMeshLoad(path, 0);
			return;
		}
		iter += sizeof(float) * 3 * sizes[0];
	}

	float* nrm = (float*)iter;
	if (sizes[1] != 0) {
		if (ReadFile(hFile, nrm, sizeof(float) * 3 * sizes[1], &bytesRead, NULL) == 0) {
			CloseHandle(hFile);
			onMeshLoad(path, 0);
			return;
		}
		iter += sizeof(float) * 3 * sizes[1];
	}

	float* tex = (float*)iter;
	if (sizes[2] != 0) {
		if (ReadFile(hFile, tex, sizeof(float) * 2 * sizes[2], &bytesRead, NULL) == 0) {
			CloseHandle(hFile);
			onMeshLoad(path, 0);
			return;
		}
		iter += sizeof(float) * 2 * sizes[1];
	}

	result->tan = (float*)iter;
	Internal::CalculateTangentArray(sizes[0], (vec3*)pos, (vec3*)nrm, (vec2*)tex, (vec3*)result->tan);

	result->numTan = sizes[0];
	result->numPos = sizes[0];
	result->numNrm = sizes[1];
	result->numTex = sizes[2];
	result->pos = pos;
	result->nrm = nrm;
	result->tex = tex;

	CloseHandle(hFile);
	onMeshLoad(path, result);
}

void LoadTexture(const char* path, OnTextureLoaded onTextureLoad) {
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD bytesRead = 0;

	unsigned int sizes[3];

	if (hFile == INVALID_HANDLE_VALUE) {
		onTextureLoad(path, 0);
		return;
	}
	if (ReadFile(hFile, sizes, sizeof(unsigned int) * 3, &bytesRead, NULL) == 0) {
		CloseHandle(hFile);
		onTextureLoad(path, 0);
		return;
	}

	unsigned int mem_needed = sizeof(TextureFile) + sizeof(unsigned char) * sizes[0] * sizes[1] * sizes[2];
	void* mem = VirtualAlloc(0, mem_needed + 1, MEM_COMMIT, PAGE_READWRITE);
	unsigned char* iter = (unsigned char*)mem;

	TextureFile* result = (TextureFile*)iter;
	iter += sizeof(TextureFile);

	unsigned char* texData = (unsigned char*)iter;
	if (sizes[0] * sizes[1] * sizes[2] != 0) {
		if (ReadFile(hFile, texData, sizeof(unsigned char) * sizes[0] * sizes[1] * sizes[2], &bytesRead, NULL) == 0) {
			CloseHandle(hFile);
			onTextureLoad(path, 0);
			return;
		}
	}
	
	CloseHandle(hFile);

	result->width = sizes[0];
	result->height = sizes[1];
	result->channels = sizes[2];
	result->data = texData;

	onTextureLoad(path, result);
}

void ReleaseMesh(MeshFile* file) {
	VirtualFree(file, 0, MEM_RELEASE);
}

void ReleaseTexture(TextureFile* file) {
	VirtualFree(file, 0, MEM_RELEASE);
}

void ReleaseText(TextFile* file) {
	VirtualFree(file, 0, MEM_RELEASE);
}