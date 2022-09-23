#include "GraphicsWASM.cpp"
#include "mem.cpp"

Memory::Allocator* wasmGraphics_GlobalAllocator = 0;

export void wasmGraphics_SetGlobalAllocator(Memory::Allocator* alloc) {
    wasmGraphics_GlobalAllocator = alloc;
}

export void* wasmGraphics_AllocateMem(u32 bytes) {
	return wasmGraphics_GlobalAllocator->Allocate(bytes, 0, "JS bridge");
}
 
export void wasmGraphics_ReleaseMem(void* mem) {
    wasmGraphics_GlobalAllocator->Release(mem, "JS bridge");
}

typedef void* (*fpWasmGraphicsAllocateMem)(u32 bytes);
typedef void (*fpWasmGraphicsReleaseMem)(void* mem);

export fpWasmGraphicsAllocateMem wasmGraphics_GetAlloceFunction() {
    return wasmGraphics_AllocateMem;
}

export fpWasmGraphicsReleaseMem wasmGraphics_GetReleaseFunction() {
    return wasmGraphics_ReleaseMem;
}

// TODO: File loader API
// TODO: Sample.cpp