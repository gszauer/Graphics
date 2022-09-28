// https://stackoverflow.com/questions/72568387/why-is-an-objects-constructor-being-called-in-every-exported-wasm-function
extern "C" void __wasm_call_ctors(void);
__attribute__((export_name("_initialize")))
extern "C" void _initialize(void) {
    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();
}

#if 0
static __attribute__((always_inline)) inline void* memcpy (void *dest, const void *src, unsigned int len) {
  char *d = (char*)dest;
  const char *s = (const char*)src;
  while (len--)
    *d++ = *s++;
  return dest;
}
#endif

#include "GraphicsWASM.cpp"
#include "mem.cpp"
#include "math.cpp"

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

#include "FileLoadersWASM.cpp"
#include "sample.cpp"

export void StartSample(Graphics::Device* gfx) {
    Graphics::Dependencies deps;
    deps.Request = wasmGraphics_AllocateMem;
    deps.Release = wasmGraphics_ReleaseMem;
    GraphicsAssert(gfx != 0, "Can't have null GFX");
    Initialize(&deps, gfx);
}

export void UpdateSample(Graphics::Device* gfx, float dt) {
    Update(gfx, dt);
}

export void RenderSample(Graphics::Device * gfx, int x, int y, int w, int h) {
    if (isFinishedInitializing) {
        Render(gfx, x, y, w, h);
    }
}
