// https://stackoverflow.com/questions/72568387/why-is-an-objects-constructor-being-called-in-every-exported-wasm-function
extern "C" void __wasm_call_ctors(void);
__attribute__((export_name("_initialize")))
extern "C" void _initialize(void) {
    // The linker synthesizes this to call constructors.
    __wasm_call_ctors();
}




 typedef unsigned int u32;
 static_assert (sizeof(u32) == 4, "u32 should be defined as a 4 byte type");




 typedef float f32;
 static_assert (sizeof(f32) == 4, "f32 should be defined as a 4 byte type");


namespace Graphics {
 // The only external dependancy is memory allocation
 typedef void* (*fpRequest)(u32 bytes);
 typedef void (*fpRelease)(void* mem);

 struct Dependencies {
  fpRequest Request; // malloc
  fpRelease Release; // free

  inline Dependencies() {
   Request = 0;
   Release = 0;
  }
 };

 class Buffer;
 class Shader;
 class Device;
 class Texture;
 class VertexLayout;
 class FrameBuffer;

 enum class Filter {
  Nearest = 0,
  Linear = 1
 };

 enum class WrapMode {
  Repeat = 0,
  Clamp = 1
 };

 enum class DepthFunc {
  Always = 1,
  Never = 2,
  Less = 3,
  Equal = 4,
  LEqual = 5,
  Greater = 6,
  GEqual = 7,
  NotEqual = 8
 };

 enum class DrawMode {
  Points = 0,
  Lines = 1,
  LineStrip = 2,
  Triangles = 3,
  TriangleStrip = 4,
  TriangleFan = 5
 };

 enum class BufferType {
  Float32 = 0,
  Int8 = 1,
  UInt8 = 2,
  Int16 = 3,
  UInt16 = 4,
  Int32 = 5,
  UInt32 = 6
 };

 enum class UniformType {
  Int1 = 0,
  Int2 = 1,
  Int3 = 2,
  Int4 = 3,
  Float1 = 4,
  Float2 = 5,
  Float3 = 6,
  Float4 = 7,
  Float9 = 8,
  Float16 = 9
 };

 enum class BlendEquation {
  Add = 0,
  Subtract = 1,
  ReverseSubtract = 2,
  Min = 3,
  Max = 4
 };

 enum class BlendFunction {
  Zero = 1,
  One = 2,
  SrcColor = 3,
  OneMinusSrcColor = 4,
  DstColor = 5,
  OneMinusDstColor = 6,
  SrcAlpha = 7,
  OneMinusSrcAlpha = 8,
  DstAlpha = 9,
  OneMinusDstAlpha = 10,
  ConstColor = 11,
  OneMinusConstColor = 12,
  ConstAlpha = 13,
  OneMinusconstAlpha = 14,
  SrcAlphaSaturate = 15
 };

 enum class CullFace {
  Off = 0,
  Back = 1, // Default
  Front = 2,
  FrontAndBack = 3
 };

 enum class FaceWind {
  CounterClockwise = 0, // Default
  Clockwise = 1
 };

 enum class TextureFormat { // Rename to texture format
  R8 = 0,
  RG8 = 1,
  RGB8 = 2,
  RGBA8 = 3,

  R32F = 4,
  RG32F = 5,
  RGB32F = 6,
  RGBA32F = 7,

  Depth = 8
 };

 struct Index { // Uniform / Attribute Index
  u32 id;
  bool valid;

  inline Index(u32 _id = 0, bool _valid = false) {
   id = _id;
   valid = _valid;
  }

  inline Index(const Index& o) {
   id = o.id;
   valid = o.valid;
  }

  inline Index& operator=(const Index& o) {
   id = o.id;
   valid = o.valid;
   return *this;
  }
 };

 struct Sampler {
  WrapMode wrapS;
  WrapMode wrapT;
  WrapMode wrapR; // Only used for cubemaps
  Filter min; // Downscale
  Filter mip; // Mipmap transition
  Filter mag; // Upscale

  inline Sampler(WrapMode _wrapS = WrapMode::Repeat, WrapMode _wrapT = WrapMode::Repeat, WrapMode _wrapR = WrapMode::Repeat, Filter _min = Filter::Linear, Filter _mip = Filter::Linear, Filter _mag = Filter::Linear) {
   wrapS = _wrapS;
   wrapT = _wrapT;
   wrapR = _wrapR;
   min = _min;
   mip = _mip;
   mag = _mag;
  }

  inline Sampler(Filter _min, Filter _mip = Filter::Linear, Filter _mag = Filter::Linear) {
   wrapS = WrapMode::Repeat;
   wrapT = WrapMode::Repeat;
   wrapR = WrapMode::Repeat;
   min = _min;
   mip = _mip;
   mag = _mag;
  }

  inline Sampler(WrapMode _wrap) {
   wrapS = _wrap;
   wrapT = _wrap;
   wrapR = _wrap;
   min = Filter::Linear;
   mip = Filter::Linear;
   mag = Filter::Linear;
  }
 };

 struct BufferView {
  u32 NumberOfComponents; // (1) float, (2) vec2, (3) vec3, (4) vec4
  u32 StrideInBytes;
  BufferType Type; // float or int
  u32 DataOffsetInBytes; // pointer argument to glVertexAttribLPointer

  inline BufferView( u32 _numberOfComponents = 0,
       u32 _strideInBytes = 0,
       BufferType _type = BufferType::Float32,
       u32 _dataOffsetInBytes = 0) {
   NumberOfComponents = _numberOfComponents;
   StrideInBytes = _strideInBytes;
   Type = _type;
   DataOffsetInBytes = _dataOffsetInBytes;
  }
 };

 // I'm going to keep the concept of a depth texture for now, and check
 // how to use the extension in webgl. If htat doesn't work, then using a
 // renderbuffer when TextureFormat::Depth instead of a texture object
 // but i don't think compatibility is going to be that big of an issue.
 class Texture { // These are texture objects
  friend class Device;
  friend class FrameBuffer;
 protected:
  u32 mId;
  u32 mWidth;
  u32 mHeight;
  u32 mUserData;
  bool mIsMipMapped;
  bool mIsCubeMap;
  TextureFormat mInternalFormat;

  u32 mCachedMin; // Default: GL_NEAREST_MIPMAP_LINEAR
  u32 mCachedMag; // Default: GL_LINEAR
  u32 mCachedS; // Default = GL_REPEAT
  u32 mCachedR; // Default = GL_REPEAT
  u32 mCachedT; // Default = GL_REPEAT

  Texture* mAllocPrev;
  Texture* mAllocNext;
  Device* mOwner;
 private:
  Texture() = delete;
  ~Texture() = delete;
  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;
 public:
  void Set(void* data, TextureFormat dataFormat, u32 width, u32 height, bool genMipMaps);
  void SetPCM(bool pcm);

  inline void Set(u32 width, u32 height) {
   TextureFormat dataFormat = TextureFormat::RGBA8;
   Set(0, mInternalFormat, width, height, false);
  }

  void SetCubemap(void* rightData, void* leftData, void* topData, void* bottomData, void* backData, void* frontData,
   u32 width, u32 height, TextureFormat format, bool genMipMaps);

  inline u32 GetWidth() {
   return mWidth;
  }

  inline u32 GetHeight() {
   return mHeight;
  }

  inline TextureFormat GetFormat() {
   return mInternalFormat;
  }

  inline void SetUserData(u32 data) {
   mUserData = data;
  }

  inline u32 GetUserData() {
   return mUserData;
  }
 };

 namespace Internal {
  struct TextureUnit {
   Graphics::Index index;
   Graphics::Texture* texture; // Null or bound texture
   u32 target; // TEXTURE_2D, TEXTURE_3D, etc...
  };
 }

 class Buffer {
  friend class Device;
  friend class VertexLayout;
 protected:
  u32 mId;
  bool mIndexBuffer;
  u32 mUserData;

  Buffer* mAllocPrev;
  Buffer* mAllocNext;
  Device* mOwner;
 private:
  Buffer() = delete;
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;
  ~Buffer() = delete;
 public:
  void Set(void* inputArray, u32 arraySizeInBytes, bool _static = true);
  void Reset();

  inline void SetUserData(u32 data) {
   mUserData = data;
  }

  inline u32 GetUserData() {
   return mUserData;
  }

  inline bool IsIndexBuffer() {
   return mIndexBuffer;
  }
 };

 // Note to self in the future:
 // The way this would work is that a MeshRenderer has a list of materials
 // Each material has a shader. For each material, the mesh renderer needs
 // to create a vertex layout. Then, when drawing a MeshRenderer, loop trough
 // all of it's materials. For each material: bind the shader, and the vertex
 // layout. 
 // To do things like normal mapping, A mapping from VertexLayout to arbitrary
 // shaders / materials needs to be doable. 
 class VertexLayout {
  friend class Device;
 protected:
  u32 mId;
  bool mHasIndexBuffer;
  u32 mUserData;
  BufferType mIndexBufferType;

  VertexLayout* mAllocPrev;
  VertexLayout* mAllocNext;
  Device* mOwner;
 private:
  VertexLayout() = delete;
  VertexLayout(const VertexLayout&) = delete;
  VertexLayout& operator=(const VertexLayout&) = delete;
  ~VertexLayout() = delete;
 public:
  void Set(const Index& index, const Buffer& buffer, const BufferView& view, u32 instanceDivisor = 0);
  void Set(const Buffer& indexBuffer, const BufferType& indexType);
  void Reset();

  inline void SetUserData(u32 data) {
   mUserData = data;
  }

  inline u32 GetUserData() {
   return mUserData;
  }
 };

 class Shader {
  friend class Device;
 protected:
  u32 mProgram;
  u32 mUserData;
  Shader* mAllocPrev;
  Shader* mAllocNext;
  Device* mOwner;
 private:
  Shader() = delete;
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  ~Shader() = delete;
 public:
  Index GetAttribute(const char* name);
  Index GetUniform(const char* name);

  inline void SetUserData(u32 data) {
   mUserData = data;
  }

  inline u32 GetUserData() {
   return mUserData;
  }

  /* I added support for the shader knowing it's uniform names here,
		* with the idea that a MeshRenderer would need to map the shader to
		* some vertex buffer format. That's not really important tough. If the
		* standard streams are known  "ie, position, normal, tangent, etc"
		* then i can just get their Index-es and see if the index is valid
		* if it is, bind it to the fbo, it it isn't, don't. Anyway, i already added
		* the code to get the info, so i'm going to keep it alive without 
		* exposing it. If it's not needed, i'll emove it from the final version */
 };

 class FrameBuffer {
  friend class Device;
 protected:
  u32 mId;

  union {
   struct {
    Texture* mColor0;
    Texture* mColor1;
    Texture* mColor2;
    Texture* mColor3;
    Texture* mColor4;
    Texture* mColor5;
   };
   Texture* mColor[6];
  };

  Texture* mDepth;
  u32 mReadBufferConfig;

  u32 mUserData;

  FrameBuffer* mAllocPrev;
  FrameBuffer* mAllocNext;
  Device* mOwner;
 private:
  FrameBuffer() = delete;
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer& operator=(const FrameBuffer&) = delete;
  ~FrameBuffer() = delete;
 public:
  void AttachColor(Texture& color, u32 attachmentIndex = 0);
  void AttachDepth(Texture& depth, bool pcm = true);

  void Resize(u32 width, u32 height);
  bool IsValid();

  u32 TargetCount();
  u32 GetWidth();
  u32 GetHeight();

  void ResolveTo(FrameBuffer* target, Filter filter, bool color, bool depth, u32 x0, u32 y0, u32 x1, u32 y1, u32 x2, u32 y2, u32 x3, u32 y3);
  void ResolveTo(FrameBuffer* target, Filter filter = Filter::Nearest, bool color = true, bool depth = true);

  inline void SetUserData(u32 data) {
   mUserData = data;
  }

  inline u32 GetUserData() {
   return mUserData;
  }
 };

 class Device {
  friend class FrameBuffer;
  friend Device* Initialize(Device& outDevice, Dependencies& platform);
  friend void Shutdown(Device& device);
 protected:
  Dependencies mPlatform;
  u32 mUserData;
  u32 mViewportRect[4];

  u32 mBoundProgram;
  u32 mBoundFrameBuffer;

  // Texture state
  Internal::TextureUnit mBoundTextures[32];

  // Cull state
  CullFace mFaceCulling; // Default back
  FaceWind mWindingOrder; // Default CCW

  // Blend State
  bool mBlend; // Default: false
  f32 mBlendColor[4]; // Default: 0, 0, 0, 0
  BlendFunction mBlendDstAlpha; // Default BlendFunction::Zero
  BlendFunction mBlendDstRGB; // Default BlendFunction::Zero
  BlendEquation mBlendEquationAlpha; // Default: BlendEquation::Add
  BlendEquation mBlendEquationRGB; // Default: BlendEquation::Add
  BlendFunction mBlendSrcAlpba; // Default BlendFunction::One
  BlendFunction mBlendSrcRGB; // Default BlendFunction::One

  // Depth state
  bool mDepth; // true
  DepthFunc mDepthFunc; // less
  f32 mDepthRange[2]; // 0, 1

  // Scissor state
  bool mScissor;
  u32 mScissorRect[4];

  // Memory tracking
  Texture* mAllocatedTextures;
  Buffer* mAllocatedBuffers;
  VertexLayout* mAllocatedStates;
  Shader* mAllocatedShaders;
  FrameBuffer* mAllocatedFrameBuffers;
 private:
  Device() = delete;
  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;
  ~Device() = delete;
 public:
  inline void* Allocate(u32 bytes) {
   if (mPlatform.Request != 0) {
    return mPlatform.Request(bytes);
   }
   return 0;
  }
  inline void Release(void* mem) {
   if (mPlatform.Release != 0) {
    mPlatform.Release(mem);
   }
  }
 public:
  Shader* CreateShader(const char* vertex, const char* fragment);
  void Destroy(Shader* shader);

  Buffer* CreateBuffer();
  Buffer* CreateIndexBuffer();
  inline Buffer* CreateBuffer(void* array, u32 sizeInBytes, bool _static = true) {
   Buffer* result = CreateBuffer();
   result->Set(array, sizeInBytes, _static);
   return result;
  }
  inline Buffer* CreateIndexBuffer(void* array, u32 sizeInBytes, bool _static = true) {
   Buffer* result = CreateIndexBuffer();
   result->Set(array, sizeInBytes, _static);
   return result;
  }
  void Destroy(Buffer* buff);

  VertexLayout* CreateVertexLayout();
  void Destroy(VertexLayout* map);

  FrameBuffer* CreateFrameBuffer();
  void Destroy(FrameBuffer* buffer);

  Texture* CreateTexture(TextureFormat format);

  inline Texture* CreateTexture(TextureFormat format, u32 width, u32 height, void* data, TextureFormat dataFormat, bool genMipMaps) {
   Texture* result = CreateTexture(format);
   result->Set(data, dataFormat, width, height, genMipMaps);
   return result;
  }
  inline Texture* CreateTexture(TextureFormat format, u32 width, u32 height) {
   Texture* result = CreateTexture(format);
   result->Set(width, height);
   return result;
  }
  void Destroy(Texture* buff);

  void Bind(Shader* shader); // Binds shader & allows for Bind(0)
  inline void Bind(Shader& shader) {
   Bind(&shader);
  }

  // Frame buffers should persist between Bind calls, even if null is bound
  void SetRenderTarget(FrameBuffer* frameBuffer);
  inline void SetRenderTarget(FrameBuffer& frameBuffer) {
   SetRenderTarget(&frameBuffer);
  }

  void Bind(Index& slot, UniformType type, void* data, u32 count = 1); // Binds uniform
  void Bind(Index& uniformSlot, Texture& texture, Sampler& sampler); // Binds texture (uniform still)

  void Draw(const VertexLayout& attributes, DrawMode drawMode, u32 startIndex, u32 indexCount, u32 instanceCount = 1);
 public: // State management
  // Blend state
  void SetBlendState(bool blend, f32* optBlendColor,
   BlendFunction blendDstRgb, BlendFunction blendDstAlpha,
   BlendEquation blendEquationRgb, BlendEquation blendEquationAlpha,
   BlendFunction blendSrcRgb, BlendFunction blendSrcAlpha);

  inline void SetBlendState(f32* color, BlendFunction blendDst, BlendEquation blendEq, BlendFunction blendSrc) {
   SetBlendState(true, color, blendDst, blendDst, blendEq, blendEq, blendSrc, blendSrc);
  }
  inline void SetBlendState(BlendFunction dst, BlendEquation eq, BlendFunction src) {
   SetBlendState(true, 0, dst, dst, eq, eq, src, src);
  }
  inline void SetBlendState(BlendFunction dst, BlendFunction src) {
   SetBlendState(true, 0, dst, dst, BlendEquation::Add, BlendEquation::Add, src, src);
  }
  inline void SetBlendState(BlendEquation eq) {
   SetBlendState(true, 0, BlendFunction::Zero, BlendFunction::Zero, eq, eq, BlendFunction::One, BlendFunction::One);
  }
  inline void SetBlendState(bool blend) {
   SetBlendState(blend, 0, BlendFunction::Zero, BlendFunction::Zero, BlendEquation::Add, BlendEquation::Add, BlendFunction::One, BlendFunction::One);
  }
  // End Blend state

  // Cull state
  void SetFaceVisibility(CullFace cull, FaceWind wind = FaceWind::CounterClockwise);
  // End cull state

  // Depth State
  void SetDepthState(bool enable, DepthFunc depthFunc, f32* depthRange = 0);
  inline void SetDepthState(DepthFunc depthFunc) {
   SetDepthState(true, depthFunc, 0);
  }
  inline void SetDepthState(bool enable) {
   SetDepthState(enable, DepthFunc::Less, 0);
  }
  // End depth state

  // Scissor State
  void SetScissorState(bool enable, u32 x, u32 y, u32 w, u32 h);
  inline void SetScissorState(bool enable) {
   SetScissorState(enable, mScissorRect[0], mScissorRect[1], mScissorRect[2], mScissorRect[3]);
  }
  // End Scissor state


  void WriteMask(bool r, bool g, bool b, bool a, bool depth);
  void SetViewport(u32 x, u32 y, u32 w, u32 h);

  void Clear(f32 r, f32 g, f32 b, f32 depth);
  void Clear(f32 r, f32 g, f32 b);
  void Clear(f32 depth);
  void Clear(bool color, bool depth);

  inline void SetUserData(u32 data) {
   mUserData = data;
  }

  inline u32 GetUserData() {
   return mUserData;
  }

  inline Dependencies* GetPlatform() {
   return &mPlatform;
  }
 };

 Device* Initialize(Device& outDevice, Dependencies& platform);
 inline Device* Initialize(Dependencies& platform) {
  Device* result = (Device*)platform.Request(sizeof(Graphics::Device));
  Initialize(*result, platform);
  return result;
 }
 void Shutdown(Device& device);
}
 typedef int i32;
 static_assert (sizeof(i32) == 4, "i32 should be defined as a 4 byte type");


typedef int GLenum;

extern "C" void wasmGraphics_Log(const char* loc, int locLen, const char* msg, int msgLen);
extern "C" void wasmGraphics_SetTexturePCM(int glTextureId, int glTextureAttachTarget, int glCompareMode, int glCompareFunc);
extern "C" void wasmGraphics_TextureSetData(int glTextureId, int glInternalFormat, int width, int height, int glDataFormat, int glDataFormatType, void* data, bool genMipMaps);
extern "C" void wasmGraphics_TextureSetCubemap(int glTextureId, int glInternalFormat, int width, int height, int glDataFormat, int glDataType, void* rightData, void* leftData, void* topData, void* bottomData, void* backData, void* frontData, bool genMipMaps);
extern "C" void wasmGraphics_DeviceSetFaceVisibility(bool enableCullFace, bool disableCullFace, int cullFaceType, bool changeFace, int faceWind);
extern "C" void wasmGraphics_DeviceClearRGBAD(float r, float g, float b, float d);
extern "C" void wasmGraphics_SetDepthState(bool changeDepthState, int depthState, bool changeDepthFunc, int func, bool changeDepthRange, float depthRangeMin, float depthRangeMax);
extern "C" void wasmGraphics_DeviceClearBufferBits(bool color, bool depth);
extern "C" void wasmGraphics_DeviceWriteMask(bool r, bool g, bool b, bool a, bool depth);
extern "C" void wasmGraphics_SetGLBlendFuncEnabled(bool state);
extern "C" void wasmGraphics_UpdateGLBlendColor(float r, float g, float b, float a);
extern "C" void wasmGraphics_ChangeGLBlendFuncSame(int srcRgb, int dstRgb);
extern "C" void wasmGraphics_ChangeGLBlendFuncSeperate(int srcRgb, int dstRgb, int srcAlpha, int dstAlpha);
extern "C" void wasmGraphics_ChangeGLBlendEquation(int rgbEquation);
extern "C" void wasmGraphics_ChangeGLBlendEquationSeparate(int rgbEquation, int alphaEquation);
extern "C" void wasmGraphics_DeviceClearColor(float r, float g, float b);
extern "C" void wasmGraphics_DeviceClearDepth(float depth);
extern "C" void wasmGraphics_SetGLViewport(int x, int y, int w, int h);
extern "C" void wasmGraphics_DeviceSetScissorState(bool enableScissor, bool disableScissor, bool updateRect, int x, int y, int w, int h);
extern "C" int wasmGraphics_GLGenFrameBuffer();
extern "C" void wasmGraphics_DestroyFrameBuffer(int bufferId);
extern "C" int wasmGraphics_GLGenBuffer();
extern "C" void wasmGraphics_GLDestroyBuffer(int bufferId);
extern "C" int wasmGraphics_GLCreateVAO();
extern "C" void wasmGraphics_GLDestroyVAO(int vaoID);
extern "C" int wasmGraphics_CreateTexture();
extern "C" void wasmGraphics_GLDestroyTexture(int texId);
extern "C" void wasmGraphics_GLDestroyShader(int shaderId);
extern "C" void wasmGraphics_DeviceSetRenderTarget(int frameBufferId, int numAttachments);
extern "C" void wasmGraphics_DeviceDraw(int glVao, bool indexed, int instanceCount, int drawMode, int startIndex, int indexCount, int bufferType);
extern "C" void wasmGraphics_VertexLayoutSet(int glVaoId, int glBufferId, int slotId, int numComponents, int type, int stride, int offset, int divisor);
extern "C" void wasmGraphics_VertexLayoutSetIndexBuffer(int glVaoId, int glElementArrayBufferId);
extern "C" void wasmGraphics_FramebufferAttachColor(int attachTarget, int frameBufferId, int textureId, int attachmentIndex);
extern "C" void wasmGraphics_FrameBufferAttachDepth(int target, int frameBufferId, int textureId, bool pcm);
extern "C" bool wasmGraphics_FrameBufferIsValid(int frameBufferId);
extern "C" void wasmGraphics_FrameBufferResolveTo(int readBuffer, int drawBuffer, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, bool colorBit, bool depthBit, int filter);
extern "C" int wasmGraphics_BufferReset(int bufferId);
extern "C" void wasmGraphics_BufferSet(bool indexBuffer, int bufferId, int arraySizeInByfes, void* inputArray, bool isStatic);
extern "C" int wasmGraphics_ShaderGetUniform(int program, const char* name, int name_len);
extern "C" int wasmGraphics_ShaderGetAttribute(int program, const char* name, int name_len);
extern "C" int wasmGraphics_ResetVertexLayout(int id);
extern "C" void wasmGraphics_BindVAO(int vaoId);
extern "C" void wasmGraphics_DeviceBindTexture(int textureUnitEnum, int textureUnitNumber, int textureTarget, int textureId, int uniformSlot, int minFilter, int magFilter, int wrapS, int wrapT, int wrapR, bool updateSampler);
extern "C" void wasmGraphics_DeviceSetUniform(int type, int slotId, int count, void* data);
extern "C" void wasmGraphics_DeviceBindShader(int programId, unsigned int boundTextures);
extern "C" void wasmGraphics_SetDefaultGLState();
extern "C" void wasmGraphics_GetScissorAndViewport(void* scissorPtr, void* viewPtr); // Might need to make a "SetRect" function to support in wasm
extern "C" int wasmGraphics_CompileShader(const void* vShader, int vShaderLen, const void* fShader, int fShaderLen);

// https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/Constants#standard_webgl_1_constants
// Helpers
namespace Graphics {
 namespace Internal {
  void Assert(bool cond, const char* location, const char* msg) {
   if (cond == false) {
    u32 msgLen = 0;
    for (const char* i = msg; msg != 0 && *i != '\0'; ++i, ++msgLen);
    u32 locLen = 0;
    for (const char* i = location; location != 0 && *i != '\0'; ++i, ++locLen);

    wasmGraphics_Log(location, locLen, msg, msgLen);
    __builtin_trap();
   }
  }

  struct TextureFormatResult {
   GLenum dataType;
   GLenum dataFormat;
  };

  struct ShaderCompileResult {
   bool success;
   u32 program;
   char* uniforms;
   char* attributes;
   u32 numAttribs;
   u32 numUniforms;

   ShaderCompileResult() {
    success = false;
    program = 0;
    uniforms = 0;
    attributes = 0;
    numAttribs = 0;
    numUniforms = 0;
   }
  };

  inline GLenum DepthFuncToEnum(DepthFunc f) {
   if (f == DepthFunc::Always) {
    return 0x0207;
   }
   else if (f == DepthFunc::Never) {
    return 0x0200;
   }
   else if (f == DepthFunc::Equal) {
    return 0x0202;
   }
   else if (f == DepthFunc::LEqual) {
    return 0x0203;
   }
   else if (f == DepthFunc::Greater) {
    return 0x0204;
   }
   else if (f == DepthFunc::GEqual) {
    return 0x0206;
   }
   else if (f == DepthFunc::NotEqual) {
    return 0x0205;
   }

   return 0x0201;
  }

  inline GLenum GetTextureUnit(u32 index) {
   Graphics::Internal::Assert(index <= 32, "On line: " "219" ", in file: " "./GraphicsWASM.cpp", "Only supports up to GL_TEXTURE16");

   return 0x84C0 + index;
  }

  inline GLenum BlendEqToEnum(BlendEquation b) {
   if (b == BlendEquation::Subtract) {
    return 0x800A;
   }
   else if (b == BlendEquation::ReverseSubtract) {
    return 0x800B;
   }
   else if (b == BlendEquation::Min) {
    return 0x8007;
   }
   else if (b == BlendEquation::Max) {
    return 0x8008;
   }

   return 0x8006;
  }

  inline GLenum BlendfuncToEnum(BlendFunction b) {
   GLenum result = 0;
   if (b == BlendFunction::One) {
    result = 1;
   }
   else if (b == BlendFunction::SrcColor) {
    result = 0x0300;
   }
   else if (b == BlendFunction::OneMinusSrcColor) {
    result = 0x0301;
   }
   else if (b == BlendFunction::DstColor) {
    result = 0x0306;
   }
   else if (b == BlendFunction::OneMinusDstColor) {
    result = 0x0307;
   }
   else if (b == BlendFunction::SrcAlpha) {
    result = 0x0302;
   }
   else if (b == BlendFunction::OneMinusSrcAlpha) {
    result = 0x0303;
   }
   else if (b == BlendFunction::DstAlpha) {
    result = 0x0304;
   }
   else if (b == BlendFunction::OneMinusDstAlpha) {
    result = 0x0305;
   }
   else if (b == BlendFunction::ConstColor) {
    result = 0x8001;
   }
   else if (b == BlendFunction::OneMinusConstColor) {
    result = 0x8002;
   }
   else if (b == BlendFunction::ConstAlpha) {
    result = 0x8003;
   }
   else if (b == BlendFunction::OneMinusconstAlpha) {
    result = 0x8004;
   }
   else if (b == BlendFunction::SrcAlphaSaturate) {
    result = 0x0308;
   }
   return result;
  }

  inline GLenum DrawModeToEnum(DrawMode drawMode) {
   GLenum mode = 0x0004;
   if (drawMode == DrawMode::Points) {
    mode = 0x0000;
   }
   else if (drawMode == DrawMode::Lines) {
    mode = 0x0001;
   }
   else if (drawMode == DrawMode::LineStrip) {
    mode = 0x0003;
   }
   else if (drawMode == DrawMode::TriangleStrip) {
    mode = 0x0005;
   }
   else if (drawMode == DrawMode::TriangleFan) {
    mode = 0x0006;
   }
   return mode;
  }

  inline GLenum TextureGetInternalFormatFromEnum(TextureFormat component) {
   if (component == TextureFormat::R8) {
    return 0x8229;
   }
   else if (component == TextureFormat::RG8) {
    return 0x822B;
   }
   if (component == TextureFormat::RGB8) {
    return 0x8051;
   }
   if (component == TextureFormat::RGBA8) {
    return 0x8058;
   }

   if (component == TextureFormat::R32F) {
    return 0x822E;
   }
   else if (component == TextureFormat::RG32F) {
    return 0x8230;
   }
   if (component == TextureFormat::RGB32F) {
    return 0x8815;
   }
   if (component == TextureFormat::RGBA32F) {
    return 0x8814;
   }

   return 0x81A6; // Default to depth i guess
  }

  inline TextureFormatResult TextureGetDataFormatFromEnum(TextureFormat component) {
   GLenum dataFormat = 0x81A6;
   GLenum dataType = 0x1405;

   if (component == TextureFormat::R8) {
    dataFormat = 0x1903;
    dataType = 0x1401;
   }
   else if (component == TextureFormat::RG8) {
    dataFormat = 0x822B;
    dataType = 0x1401;
   }
   if (component == TextureFormat::RGB8) {
    dataFormat = 0x1907;
    dataType = 0x1401;
   }
   if (component == TextureFormat::RGBA8) {
    dataFormat = 0x1908;
    dataType = 0x1401;
   }

   if (component == TextureFormat::R32F) {
    dataFormat = 0x1903;
    dataType = 0x1406;
   }
   else if (component == TextureFormat::RG32F) {
    dataFormat = 0x8230;
    dataType = 0x1406;
   }
   if (component == TextureFormat::RGB32F) {
    dataFormat = 0x1907;
    dataType = 0x1406;
   }
   if (component == TextureFormat::RGBA32F) {
    dataFormat = 0x1908;
    dataType = 0x1406;
   }

   TextureFormatResult result;
   result.dataFormat = dataFormat;
   result.dataType = dataType;
   return result;
  }

  inline GLenum BufferTypeToEnum(BufferType bufferType) {
   GLenum type = 0x1406;

   if (bufferType == BufferType::Int8) {
    type = 0x1400;
   }
   else if (bufferType == BufferType::UInt8) {
    type = 0x1401;
   }
   else if (bufferType == BufferType::Int16) {
    type = 0x1402;
   }
   else if (bufferType == BufferType::UInt16) {
    type = 0x1403;
   }
   else if (bufferType == BufferType::Int32) {
    type = 0x1404;
   }
   else if (bufferType == BufferType::UInt32) {
    type = 0x1405;
   }

   return type;
  }

  ShaderCompileResult CompileOpenGLShader(const char* vertexSource, const char* fragmentSource, Dependencies* platform) {
   ShaderCompileResult result;
   result.program = 0;

   int vLen = 0;
   Graphics::Internal::Assert(vertexSource != 0, "On line: " "412" ", in file: " "./GraphicsWASM.cpp", "Empty vertex pointer");
   for (const char* i = vertexSource; *i != '\0'; ++i, ++vLen);
   const char* vp1 = vertexSource + 1;
   Graphics::Internal::Assert(vLen != 0, "On line: " "415" ", in file: " "./GraphicsWASM.cpp", "Empty vertex shader");

   int fLen = 0;
   Graphics::Internal::Assert(fragmentSource != 0, "On line: " "418" ", in file: " "./GraphicsWASM.cpp", "Empty fragment pointer");
   for (const char* i = fragmentSource; *i != '\0'; ++i, ++fLen);
   Graphics::Internal::Assert(fLen != 0, "On line: " "420" ", in file: " "./GraphicsWASM.cpp", "Empty fragment shader");

   result.program = wasmGraphics_CompileShader(vertexSource, vLen, fragmentSource, fLen);
   result.success = result.program != 0;
   Graphics::Internal::Assert(result.success, "On line: " "424" ", in file: " "./GraphicsWASM.cpp", "Could not compile shader");

   return result;
  }
 }
}

/// Texture
void Graphics::Texture::SetPCM(bool pcm) {
 GLenum attachTarget = 0x0DE1;
 if (mIsCubeMap) {
  attachTarget = 0x8513;
 }

 GLenum compareMode = 0;
 GLenum compareFunc = 0;
 if (pcm) {
  compareMode = 0x884E;
  compareFunc = 0x0203;
 }


 wasmGraphics_SetTexturePCM(mId, attachTarget, compareMode, compareFunc);
}

void Graphics::Texture::Set(void* data, TextureFormat dataFormat, u32 width, u32 height, bool genMipMaps) {
 GLenum internalFormat = Internal::TextureGetInternalFormatFromEnum(mInternalFormat);
 Internal::TextureFormatResult f = Internal::TextureGetDataFormatFromEnum(dataFormat);

 mWidth = width;
 mHeight = height;
 mIsCubeMap = false;
 mIsMipMapped = genMipMaps;

 wasmGraphics_TextureSetData(mId, internalFormat, width, height, f.dataFormat, f.dataType, data, genMipMaps);
}

void Graphics::Texture::SetCubemap(void* rightData, void* leftData, void* topData, void* bottomData, void* backData, void* frontData,
 u32 width, u32 height, TextureFormat texFormat, bool genMipMaps) {

 GLenum internalFormat = Internal::TextureGetInternalFormatFromEnum(mInternalFormat);
 Internal::TextureFormatResult f = Internal::TextureGetDataFormatFromEnum(texFormat);

 mWidth = width;
 mHeight = height;
 mIsCubeMap = true;
 mIsMipMapped = genMipMaps;

 wasmGraphics_TextureSetCubemap(mId, internalFormat, width, height, f.dataFormat, f.dataType, rightData, leftData, topData, bottomData, backData, frontData, genMipMaps);
}

/// Device

void Graphics::Device::SetFaceVisibility(CullFace cull, FaceWind wind) {
 bool enableCullFace = false;
 bool disableCullFace = false;
 GLenum cullFaceType = 0x0405;
 bool changeFace = false;
 GLenum faceWind = 0x0901;

 if (mFaceCulling != cull) {
  if (cull == CullFace::Back) {
   if (mFaceCulling == CullFace::Off) {
    enableCullFace = true;
   }
   cullFaceType = 0x0405;
  }
  else if (cull == CullFace::Front) {
   if (mFaceCulling == CullFace::Off) {
    enableCullFace = true;
   }
   cullFaceType = 0x0404;
  }
  else if (cull == CullFace::FrontAndBack) {
   if (mFaceCulling == CullFace::Off) {
    enableCullFace = true;
   }
   cullFaceType = 0x0408;
  }
  else { // Off
   if (mFaceCulling != CullFace::Off) {
    disableCullFace = true;
   }
  }
  mFaceCulling = cull;
 }

 if (mWindingOrder != wind) {
  changeFace = true;
  if (wind == FaceWind::CounterClockwise) {
   faceWind = 0x0901;
  }
  else {
   faceWind = 0x0900;
  }
  mWindingOrder = wind;
 }

 if (enableCullFace || disableCullFace || changeFace) {
  wasmGraphics_DeviceSetFaceVisibility(enableCullFace, disableCullFace, cullFaceType, changeFace, faceWind);
 }
}

void Graphics::Device::SetDepthState(bool enable, DepthFunc depthFunc, f32* depthRange) {
 bool changeDepthState = false;
 GLenum depthState = 0x0B71;
 bool changeDepthFunc = false;
 GLenum func = 0;
 bool changeDepthRange = false;
 float depthRangeMin = 0.0f;
 float depthRangeMax = 0.0f;

 if (mDepth != enable) {
  changeDepthState = true;
  if (enable) {
   depthState = 0x0B71;
  }
  else {
   depthState = 0; // Disabled
  }
  mDepth = enable;
 }

 if (mDepthFunc != depthFunc) {
  changeDepthFunc = true;
  func = Internal::DepthFuncToEnum(depthFunc);
  mDepthFunc = depthFunc;
 }

 if (depthRange != 0) {
  changeDepthRange = true;
  mDepthRange[0] = depthRangeMin = depthRange[0];
  mDepthRange[1] = depthRangeMax = depthRange[1];
 }

 if (changeDepthState || changeDepthFunc || changeDepthRange) {
  wasmGraphics_SetDepthState(changeDepthState, depthState, changeDepthFunc, func, changeDepthRange, depthRangeMin, depthRangeMax);
 }
}

void Graphics::Device::SetBlendState(bool blend, f32* blendColor,
 BlendFunction blendDstRgb, BlendFunction blendDstAlpha,
 BlendEquation blendEquationRgb, BlendEquation blendEquationAlpha,
 BlendFunction blendSrcRGB, BlendFunction blendSrcAlpha) {

 if (blend != mBlend) {
  if (blend) {
   wasmGraphics_SetGLBlendFuncEnabled(false);
  }
  else {
   wasmGraphics_SetGLBlendFuncEnabled(true);
  }
  mBlend = blend;
 }

 if (blendColor != 0) {
  mBlendColor[0] = blendColor[0];
  mBlendColor[1] = blendColor[1];
  mBlendColor[2] = blendColor[2];
  mBlendColor[3] = blendColor[3];
  wasmGraphics_UpdateGLBlendColor(mBlendColor[0], mBlendColor[1], mBlendColor[2], mBlendColor[3]);
 }

 if (mBlendDstAlpha != blendDstAlpha || mBlendDstRGB != blendDstRgb ||
  mBlendSrcAlpba != blendSrcAlpha || mBlendSrcRGB != blendSrcRGB) {
  GLenum srcAlpha = Internal::BlendfuncToEnum(blendSrcAlpha);
  GLenum srcRgb = Internal::BlendfuncToEnum(blendSrcRGB);
  GLenum dstAlpha = Internal::BlendfuncToEnum(blendDstAlpha);
  GLenum dstRgb = Internal::BlendfuncToEnum(blendDstRgb);

  if (blendDstAlpha == blendDstRgb && blendSrcAlpha == blendSrcRGB) { // Same
   wasmGraphics_ChangeGLBlendFuncSame(srcRgb, dstRgb);
  }
  else { // Seperate
   wasmGraphics_ChangeGLBlendFuncSeperate(srcRgb, dstRgb, srcAlpha, dstAlpha);
  }
  mBlendDstAlpha = blendDstAlpha;
  mBlendDstRGB = blendDstRgb;
  mBlendSrcAlpba = blendSrcAlpha;
  mBlendSrcRGB = blendSrcRGB;
 }

 if (mBlendEquationAlpha != blendEquationAlpha || mBlendEquationRGB != blendEquationRgb) {
  GLenum alphaEquation = Internal::BlendEqToEnum(blendEquationAlpha);
  GLenum rgbEquation = Internal::BlendEqToEnum(blendEquationRgb);

  if (blendEquationAlpha == blendEquationRgb) { // Same
   wasmGraphics_ChangeGLBlendEquation(rgbEquation);
  }
  else { // Seperate
   wasmGraphics_ChangeGLBlendEquationSeparate(rgbEquation, alphaEquation);
  }

  mBlendEquationAlpha = blendEquationAlpha;
  mBlendEquationRGB = blendEquationRgb;
 }
}

void Graphics::Device::Clear(f32 r, f32 g, f32 b, f32 depth) {
 wasmGraphics_DeviceClearRGBAD(r, g, b, depth);
}

void Graphics::Device::Clear(bool color, bool depth) {
 wasmGraphics_DeviceClearBufferBits(color, depth);
}

void Graphics::Device::WriteMask(bool r, bool g, bool b, bool a, bool depth) {
 wasmGraphics_DeviceWriteMask(r, g, b, a, depth);
}

void Graphics::Device::Clear(f32 r, f32 g, f32 b) {
 wasmGraphics_DeviceClearColor(r, g, b);
}

void Graphics::Device::Clear(f32 depth) {
 wasmGraphics_DeviceClearDepth(depth);
}

void Graphics::Device::SetViewport(u32 x, u32 y, u32 w, u32 h) {
 if (x != mViewportRect[0] || y != mViewportRect[1] ||
  w != mViewportRect[2] || h != mViewportRect[3]) {
  wasmGraphics_SetGLViewport(x, y, w, h);
  mViewportRect[0] = x;
  mViewportRect[1] = y;
  mViewportRect[2] = w;
  mViewportRect[3] = h;
 }
}

void Graphics::Device::SetScissorState(bool enable, u32 x, u32 y, u32 w, u32 h) {
 bool enableScissor = false;
 bool disableScissor = false;
 bool updateRect = false;


 if (enable != mScissor) {
  if (enable) {
   enableScissor = true;
  }
  else {
   disableScissor = true;
  }

  mScissor = enable;
 }

 if (x != mScissorRect[0] || y != mScissorRect[1] || w != mScissorRect[2] || h != mScissorRect[3]) {
  mScissorRect[0] = x;
  mScissorRect[1] = y;
  mScissorRect[2] = w;
  mScissorRect[3] = h;
 }

 if (enableScissor || disableScissor || updateRect) {
  wasmGraphics_DeviceSetScissorState(enableScissor, disableScissor, updateRect, x, y, w, h);
 }
}

Graphics::FrameBuffer* Graphics::Device::CreateFrameBuffer() {
 Graphics::FrameBuffer* result = (Graphics::FrameBuffer*)mPlatform.Request(sizeof(Graphics::FrameBuffer));
 result->mId = wasmGraphics_GLGenFrameBuffer();
 result->mColor0 = 0;
 result->mColor1 = 0;
 result->mColor2 = 0;
 result->mColor3 = 0;
 result->mColor4 = 0;
 result->mColor5 = 0;
 result->mDepth = 0;
 result->mUserData = 0;
 result->mOwner = this;
 result->mAllocPrev = 0;
 result->mAllocNext = mAllocatedFrameBuffers;
 if (mAllocatedFrameBuffers != 0) {
  mAllocatedFrameBuffers->mAllocPrev = result;
 }
 mAllocatedFrameBuffers = result;
 result->mReadBufferConfig = 0x0404;

 return result;
}

void Graphics::Device::Destroy(FrameBuffer* buffer) {
 wasmGraphics_DestroyFrameBuffer(buffer->mId);
 mBoundFrameBuffer = 0;

 if (buffer->mAllocPrev != 0) { // Not head
  buffer->mAllocPrev->mAllocNext = buffer->mAllocNext;
  if (buffer->mAllocNext != 0) {
   buffer->mAllocNext->mAllocPrev = buffer->mAllocPrev;
  }
 }
 else { // Head
  mAllocatedFrameBuffers = mAllocatedFrameBuffers->mAllocNext;
  if (mAllocatedFrameBuffers != 0) {
   mAllocatedFrameBuffers->mAllocPrev = 0;
  }
 }

 mPlatform.Release(buffer);
}

Graphics::Buffer* Graphics::Device::CreateBuffer() {
 Graphics::Buffer* result = (Graphics::Buffer*)mPlatform.Request(sizeof(Graphics::Buffer));
 result->mId = wasmGraphics_GLGenBuffer();
 result->mIndexBuffer = false;
 result->mUserData = 0;
 result->mOwner = this;
 result->mAllocPrev = 0;
 result->mAllocNext = mAllocatedBuffers;
 if (mAllocatedBuffers != 0) {
  mAllocatedBuffers->mAllocPrev = result;
 }
 mAllocatedBuffers = result;

 return result;
}

Graphics::Buffer* Graphics::Device::CreateIndexBuffer() {
 Graphics::Buffer* result = (Graphics::Buffer*)mPlatform.Request(sizeof(Graphics::Buffer));
 result->mId = wasmGraphics_GLGenBuffer();
 result->mIndexBuffer = true;
 result->mUserData = 0;
 result->mOwner = this;
 result->mAllocPrev = 0;
 result->mAllocNext = mAllocatedBuffers;
 if (mAllocatedBuffers != 0) {
  mAllocatedBuffers->mAllocPrev = result;
 }
 mAllocatedBuffers = result;

 return result;
}

void Graphics::Device::Destroy(Buffer* buff) {
 wasmGraphics_GLDestroyBuffer(buff->mId);

 if (buff->mAllocPrev != 0) { // Not head
  buff->mAllocPrev->mAllocNext = buff->mAllocNext;
  if (buff->mAllocNext != 0) {
   buff->mAllocNext->mAllocPrev = buff->mAllocPrev;
  }
 }
 else { // Head
  mAllocatedBuffers = mAllocatedBuffers->mAllocNext;
  if (mAllocatedBuffers != 0) {
   mAllocatedBuffers->mAllocPrev = 0;
  }
 }

 mPlatform.Release(buff);
}

Graphics::VertexLayout* Graphics::Device::CreateVertexLayout() {
 Graphics::VertexLayout* result = (Graphics::VertexLayout*)mPlatform.Request(sizeof(Graphics::VertexLayout));

 result->mId = wasmGraphics_GLCreateVAO();
 result->mUserData = 0;
 result->mIndexBufferType = Graphics::BufferType::UInt32;
 result->mHasIndexBuffer = false;

 result->mAllocPrev = 0;
 result->mAllocNext = mAllocatedStates;
 if (mAllocatedStates != 0) {
  mAllocatedStates->mAllocPrev = result;
 }
 mAllocatedStates = result;

 return result;
}

void Graphics::Device::Destroy(VertexLayout* map) {
 wasmGraphics_GLDestroyVAO(map->mId);

 if (map->mAllocPrev != 0) { // Not head
  map->mAllocPrev->mAllocNext = map->mAllocNext;
  if (map->mAllocNext != 0) {
   map->mAllocNext->mAllocPrev = map->mAllocPrev;
  }
 }
 else { // Head
  mAllocatedStates = mAllocatedStates->mAllocNext;
  if (mAllocatedStates != 0) {
   mAllocatedStates->mAllocPrev = 0;
  }
 }

 mPlatform.Release(map);
}

Graphics::Texture* Graphics::Device::CreateTexture(TextureFormat format) {
 Graphics::Texture* result = (Graphics::Texture*)mPlatform.Request(sizeof(Graphics::Texture));

 result->mId = wasmGraphics_CreateTexture();

 result->mWidth = 0;
 result->mHeight = 0;
 result->mInternalFormat = format;
 result->mIsMipMapped = false;
 result->mIsCubeMap = false;
 result->mUserData = 0;

 result->mCachedMin = 0x2702;
 result->mCachedMag = 0x2601;

 result->mCachedS = 0x2901;
 result->mCachedR = 0x2901;
 result->mCachedT = 0x2901;

 result->mOwner = this;
 result->mAllocPrev = 0;
 result->mAllocNext = mAllocatedTextures;
 if (mAllocatedTextures != 0) {
  mAllocatedTextures->mAllocPrev = result;
 }
 mAllocatedTextures = result;

 return result;
}

void Graphics::Device::Destroy(Texture* buff) {
 wasmGraphics_GLDestroyTexture(buff->mId);

 if (buff->mAllocPrev != 0) { // Not head
  buff->mAllocPrev->mAllocNext = buff->mAllocNext;
  if (buff->mAllocNext != 0) {
   buff->mAllocNext->mAllocPrev = buff->mAllocPrev;
  }
 }
 else { // Head
  mAllocatedTextures = mAllocatedTextures->mAllocNext;
  if (mAllocatedTextures != 0) {
   mAllocatedTextures->mAllocPrev = 0;
  }
 }

 mPlatform.Release(buff);
}

Graphics::Shader* Graphics::Device::CreateShader(const char* vertex, const char* fragment) {
 Graphics::Shader* result = 0;

 Graphics::Internal::ShaderCompileResult compileStatus =
 Graphics::Internal::CompileOpenGLShader(vertex, fragment, &mPlatform);
 Graphics::Internal::Assert(compileStatus.success, "On line: " "867" ", in file: " "./GraphicsWASM.cpp", "Failed to compile shader");

 if (compileStatus.success) {
  result = (Graphics::Shader*)mPlatform.Request(sizeof(Graphics::Shader));
  result->mProgram = compileStatus.program;
 }

 result->mUserData = 0;
 result->mOwner = this;
 result->mAllocPrev = 0;
 result->mAllocNext = mAllocatedShaders;
 if (mAllocatedShaders != 0) {
  mAllocatedShaders->mAllocPrev = result;
 }
 mAllocatedShaders = result;


 return result;
}

void Graphics::Device::Destroy(Shader* shader) {
 wasmGraphics_GLDestroyShader(shader->mProgram);
 mBoundProgram = 0;
 shader->mProgram = 0;

 if (shader->mAllocPrev != 0) { // Not head
  shader->mAllocPrev->mAllocNext = shader->mAllocNext;
  if (shader->mAllocNext != 0) {
   shader->mAllocNext->mAllocPrev = shader->mAllocPrev;
  }
 }
 else { // Head
  mAllocatedShaders = mAllocatedShaders->mAllocNext;
  if (mAllocatedShaders != 0) {
   mAllocatedShaders->mAllocPrev = 0;
  }
 }

 mPlatform.Release(shader);
}

void Graphics::Device::SetRenderTarget(FrameBuffer* frameBuffer) {
 const u32 attachments[8] = {
  0x8CE0,
  0x8CE1,
  0x8CE2,
  0x8CE3,
  0x8CE4,
  0x8CE5,
  0x8CE6,
  0x8CE7,
 };

 int frameBufferId = 0;
 int numAttachments = 0;

 if (frameBuffer != 0) {
  frameBufferId = frameBuffer->mId;
  mBoundFrameBuffer = frameBuffer->mId;
  numAttachments = frameBuffer->TargetCount();
  if (numAttachments == 0) {
   frameBuffer->mReadBufferConfig = 0;
  }
  else {
   frameBuffer->mReadBufferConfig = 0x0404;
  }
  Graphics::Internal::Assert(numAttachments <= 8, "On line: " "933" ", in file: " "./GraphicsWASM.cpp", "Only supports up to 8 color attachments");
 }
 else if (mBoundFrameBuffer != 0) {
  const u32 back_attach[1] = {0x0405};
  frameBufferId = 0;
  mBoundFrameBuffer = 0;
  numAttachments = 1;
 }

 wasmGraphics_DeviceSetRenderTarget(frameBufferId, numAttachments);
}

void Graphics::Device::Bind(Shader* shader) {
 u32 program = 0;
 if (shader != 0) {
  program = shader->mProgram;
 }

 if (mBoundProgram != program) {
  mBoundProgram = program;
  u32 bound = 0;
  { // Unbind any previously bound textures
   for (u32 i = 0; i < 32; ++i) {
    if (mBoundTextures[i].texture != 0) {
     mBoundTextures[i].texture = 0;
     mBoundTextures[i].target = 0;
     mBoundTextures[i].index.id = 0;
     mBoundTextures[i].index.valid = false;
     bound |= (1U << i);
    }
   }
  }
  wasmGraphics_DeviceBindShader(program, bound);
 }
}

void Graphics::Device::Bind(Index& slot, UniformType type, void* data, u32 count) {
 Graphics::Internal::Assert(slot.valid, "On line: " "970" ", in file: " "./GraphicsWASM.cpp", "Setting invalid uniform");
 Graphics::Internal::Assert(slot.valid || (!slot.valid && slot.id != 0), "On line: " "971" ", in file: " "./GraphicsWASM.cpp", "Something messed with slot");
 wasmGraphics_DeviceSetUniform((int)type, slot.id, count, data);
}

void Graphics::Device::Bind(Index& uniformSlot, Texture& texture, Sampler& sampler) {
 GLenum min = 0x2600;
 GLenum mag = 0x2601;

 if (texture.mIsMipMapped) {
  if (sampler.min == Filter::Nearest) {
   if (sampler.mip == Filter::Nearest) {
    min = 0x2700;
   }
   else {
    min = 0x2702;
   }
  }
  else {
   if (sampler.mip == Filter::Nearest) {
    min = 0x2701;
   }
   else {
    min = 0x2703;
   }
  }
 }
 else {
  if (sampler.min == Filter::Linear) {
   min = 0x2601;
  }
 }

 if (sampler.mag == Filter::Nearest) {
  mag = 0x2600;
 }

 GLenum wrapS = 0x2901;
 GLenum wrapT = 0x2901;
 GLenum wrapR = 0x2901;

 if (sampler.wrapS == WrapMode::Clamp) {
  wrapS = 0x812F;
 }

 if (sampler.wrapT == WrapMode::Clamp) {
  wrapT = 0x812F;
 }

 if (sampler.wrapR == WrapMode::Clamp) {
  wrapR = 0x812F;
 }

 GLenum target = 0x0DE1;
 if (texture.mIsCubeMap) {
  target = 0x8513;
 }

 // Find texture unit
 u32 textureUnit = 33;
 u32 firstFree = 33;
 for (u32 i = 0; i < 32; ++i) {
  if (mBoundTextures[i].texture != 0) { // Something is bound
   if (mBoundTextures[i].index.valid && mBoundTextures[i].index.id == uniformSlot.id) { // Re-use
    textureUnit = i;
    Graphics::Internal::Assert(target == mBoundTextures[i].target, "On line: " "1035" ", in file: " "./GraphicsWASM.cpp", "Binding invalid texture types");
    break;
   }
  }
  else if (firstFree == 33) {
   Graphics::Internal::Assert(!mBoundTextures[i].index.valid, "On line: " "1040" ", in file: " "./GraphicsWASM.cpp", "free slot should not be valid");
   firstFree = i;
  }
 }
 if (textureUnit == 33) {
  textureUnit = firstFree;
  mBoundTextures[firstFree].index = uniformSlot;
  Graphics::Internal::Assert(mBoundTextures[firstFree].index.valid, "On line: " "1047" ", in file: " "./GraphicsWASM.cpp", "Found invalid index");
  mBoundTextures[firstFree].target = target;
  mBoundTextures[firstFree].texture = &texture;
 }
 Graphics::Internal::Assert(textureUnit < 33, "On line: " "1051" ", in file: " "./GraphicsWASM.cpp", "Invalid texture unit");


 GLenum enumTextureUnit = Internal::GetTextureUnit(textureUnit);

 bool updateSampler = false;
 // Set min and mag filter
 if (texture.mCachedMin != min) {
  updateSampler = true;
  texture.mCachedMin = min;
 }
 if (texture.mCachedMag != mag) {
  updateSampler = true;
  texture.mCachedMag = mag;
 }

 // Set Wrap Mode
 if (texture.mCachedS != wrapS) {
  updateSampler = true;
  texture.mCachedS = wrapS;
 }
 if (texture.mCachedT != wrapT) {
  updateSampler = true;
  texture.mCachedT = wrapT;
 }
 if (texture.mIsCubeMap) {
  if (texture.mCachedR != wrapR) {
   updateSampler = true;
   texture.mCachedR = wrapR;
  }
 }

 wasmGraphics_DeviceBindTexture(enumTextureUnit, textureUnit, target, texture.mId, uniformSlot.id, min, mag, wrapS, wrapT, wrapR, updateSampler);
}

void Graphics::Device::Draw(const VertexLayout& attributes, DrawMode drawMode, u32 startIndex, u32 indexCount, u32 instanceCount) {
 GLenum type = Internal::BufferTypeToEnum(attributes.mIndexBufferType);
 GLenum _drawMode = Internal::DrawModeToEnum(drawMode);

 wasmGraphics_DeviceDraw(attributes.mId, attributes.mHasIndexBuffer, instanceCount, _drawMode, startIndex, indexCount, type);
}

Graphics::Device* Graphics::Initialize(Device& outDevice, Dependencies& alloc) {
 outDevice.mPlatform = alloc;
 outDevice.mBoundProgram = 0;
 outDevice.mBoundFrameBuffer = 0;

 for (u32 i = 0; i < 32; ++i) {
  outDevice.mBoundTextures[i].index.id = 0;
  outDevice.mBoundTextures[i].index.valid = false;
  outDevice.mBoundTextures[i].texture = 0;
  outDevice.mBoundTextures[i].target = 0;
 }

 outDevice.mUserData = 0;
 outDevice.mAllocatedTextures = 0;
 outDevice.mAllocatedBuffers = 0;
 outDevice.mAllocatedStates = 0;
 outDevice.mAllocatedShaders = 0;
 outDevice.mAllocatedFrameBuffers = 0;

 // Blend State
 outDevice.mBlend = false;

 outDevice.mBlendColor[0] = 0.0f;
 outDevice.mBlendColor[1] = 0.0f;
 outDevice.mBlendColor[2] = 0.0f;
 outDevice.mBlendColor[3] = 0.0f;

 outDevice.mBlendSrcAlpba = BlendFunction::One;
 outDevice.mBlendSrcRGB = BlendFunction::One;
 outDevice.mBlendDstAlpha = BlendFunction::Zero;
 outDevice.mBlendDstRGB = BlendFunction::Zero;

 outDevice.mBlendEquationAlpha = BlendEquation::Add;
 outDevice.mBlendEquationRGB = BlendEquation::Add;

 // Cull state
 outDevice.mFaceCulling = CullFace::Back;
 outDevice.mWindingOrder = FaceWind::CounterClockwise;

 // Depth state
 outDevice.mDepth = true;
 outDevice.mDepthFunc = DepthFunc::Less;
 outDevice.mDepthRange[0] = 0.0f;
 outDevice.mDepthRange[1] = 1.0f;

 // Scissor state
 wasmGraphics_SetDefaultGLState();
 wasmGraphics_GetScissorAndViewport(outDevice.mScissorRect, outDevice.mViewportRect);
 outDevice.mScissor = false;

 return &outDevice;
}

void Graphics::Shutdown(Device& device) {
 device.Bind(0);
 wasmGraphics_BindVAO(0);

 Graphics::Internal::Assert(device.mAllocatedTextures == 0, "On line: " "1150" ", in file: " "./GraphicsWASM.cpp", "Not all memory has been released");
 Graphics::Internal::Assert(device.mAllocatedBuffers == 0, "On line: " "1151" ", in file: " "./GraphicsWASM.cpp", "Not all memory has been released");
 Graphics::Internal::Assert(device.mAllocatedStates == 0, "On line: " "1152" ", in file: " "./GraphicsWASM.cpp", "Not all memory has been released");
 Graphics::Internal::Assert(device.mAllocatedShaders == 0, "On line: " "1153" ", in file: " "./GraphicsWASM.cpp", "Not all memory has been released");
 Graphics::Internal::Assert(device.mAllocatedFrameBuffers == 0, "On line: " "1154" ", in file: " "./GraphicsWASM.cpp", "Not all memory has been released");

 device.mPlatform.Request = 0;
 device.mPlatform.Release = 0;
}

// Pipeline state
void Graphics::VertexLayout::Set(const Index& index, const Buffer& buffer, const BufferView& view, u32 instanceDivisor) {
 GLenum type = Internal::BufferTypeToEnum(view.Type);
 wasmGraphics_VertexLayoutSet(mId, buffer.mId, index.id, view.NumberOfComponents, type, view.StrideInBytes, view.DataOffsetInBytes, instanceDivisor);
}

void Graphics::VertexLayout::Set(const Buffer& indexBuffer, const BufferType& indexType) {
 mHasIndexBuffer = true;
 mIndexBufferType = indexType;
 wasmGraphics_VertexLayoutSetIndexBuffer(mId, indexBuffer.mId);
}

void Graphics::VertexLayout::Reset() {
 mHasIndexBuffer = false;
 mId = wasmGraphics_ResetVertexLayout(mId);
}

// Shader

Graphics::Index Graphics::Shader::GetAttribute(const char* name) {
 u32 wasmLen = 0;
 for (const char* i = name; name != 0 && *i != '\0'; ++i, ++wasmLen);

 int location = wasmGraphics_ShaderGetAttribute(mProgram, name, wasmLen);
 Index result;
 result.id = 0;
 result.valid = false;

 if (location >= 0) {
  result.id = location;
  result.valid = true;
 }

 return result;
}

Graphics::Index Graphics::Shader::GetUniform(const char* name) {
 u32 wasmLen = 0;
 for (const char* i = name; name != 0 && *i != '\0'; ++i, ++wasmLen);

 int location = wasmGraphics_ShaderGetUniform(mProgram, name, wasmLen);

 Index result;
 result.id = 0;
 result.valid = false;

 if (location >= 0) {
  result.id = location;
  result.valid = true;
 }

 return result;
}


/// Buffer

void Graphics::Buffer::Set(void* inputArray, u32 arraySizeInBytes, bool _static) {
 wasmGraphics_BufferSet(mIndexBuffer, mId, arraySizeInBytes, inputArray, _static);
}

void Graphics::Buffer::Reset() {
 mId = wasmGraphics_BufferReset(mId);
}

u32 Graphics::FrameBuffer::TargetCount() {
 u32 count = 0;
 for (u32 i = 0; i < 6; ++i) {
  if (mColor[i] != 0) {
   count += 1;
  }
 }
 return count;
}

u32 Graphics::FrameBuffer::GetWidth() {
 for (u32 i = 0; i < 6; ++i) {
  if (mColor[i] != 0) {
   return mColor[i]->GetWidth();
  }
 }
 if (mDepth != 0) {
  return mDepth->GetWidth();
 }
 return 0;
}

u32 Graphics::FrameBuffer::GetHeight() {
 for (u32 i = 0; i < 6; ++i) {
  if (mColor[i] != 0) {
   return mColor[i]->GetHeight();
  }
 }
 if (mDepth != 0) {
  return mDepth->GetHeight();
 }
 return 0;
}

/// Frame Buffer
void Graphics::FrameBuffer::AttachColor(Texture& color, u32 attachmentIndex) {
 if (attachmentIndex > 5) { // Limit to 6 color attachments
  attachmentIndex = 5;
 }

 GLenum attachTarget = 0x0DE1;
 if (color.mIsCubeMap) {
  attachTarget = 0x8513;
 }

 color.mCachedMin = 0x2601;
 color.mCachedMag = 0x2601;

 mColor[attachmentIndex] = &color;
 mOwner->mBoundFrameBuffer = 0;
 wasmGraphics_FramebufferAttachColor(attachTarget, mId, color.mId, attachmentIndex);
}

void Graphics::FrameBuffer::AttachDepth(Texture& depth, bool pcm) {
 GLenum attachTarget = 0x0DE1;
 if (depth.mIsCubeMap) {
  attachTarget = 0x8513;
 }

 depth.mCachedMin = 0x2601;
 depth.mCachedMag = 0x2601;

 mDepth = &depth;
 mOwner->mBoundFrameBuffer = 0;

 wasmGraphics_FrameBufferAttachDepth(attachTarget, mId, depth.mId, pcm);
}

bool Graphics::FrameBuffer::IsValid() {
 mOwner->mBoundFrameBuffer = 0;
 return wasmGraphics_FrameBufferIsValid(mId);
}

void Graphics::FrameBuffer::ResolveTo(FrameBuffer* target, Filter filter, bool color, bool depth, u32 x0, u32 y0, u32 x1, u32 y1, u32 x2, u32 y2, u32 x3, u32 y3) {
 u32 targetId = 0;
 u32 targetHasColor = true;
 u32 targetHasDepth = true;
 if (target != 0) {
  targetId = target->mId;
  targetHasColor = target->TargetCount() != 0;
  targetHasDepth = target->mDepth != 0;
 }

 bool hasColor = TargetCount() != 0;
 bool hasDepth = mDepth != 0;

 bool colorBit = false;
 bool depthBit = false;
 if (hasColor && targetHasColor && color) {
  colorBit = true;
 }
 if (hasDepth && targetHasDepth && depth) {
  depthBit = true;
 }

 GLenum gl_filter = 0x2600;
 if (filter == Filter::Linear) {
  gl_filter = 0x2601;
 }

 mOwner->mBoundFrameBuffer = 0;
 wasmGraphics_FrameBufferResolveTo(mId, targetId, x0, y0, x1, y1, x2, y2, x3, y3, colorBit, depthBit, gl_filter);
}

void Graphics::FrameBuffer::ResolveTo(FrameBuffer* target, Filter filter, bool color, bool depth) {
 u32 width = GetWidth();
 u32 height = GetHeight();

 Graphics::Internal::Assert(GetWidth() == target->GetWidth(), "On line: " "1333" ", in file: " "./GraphicsWASM.cpp", "Invalid resolve");
 Graphics::Internal::Assert(GetHeight() == target->GetHeight(), "On line: " "1334" ", in file: " "./GraphicsWASM.cpp", "Invalid resolve");

 ResolveTo(target, filter, color, depth, 0, 0, width, height, 0, 0, width, height);
}

void Graphics::FrameBuffer::Resize(u32 width, u32 height) {
 for (u32 i = 0; i < 6; ++i) {
  if (mColor[i] != 0) {
   mColor[i]->Set(width, height);
  }
 }
 mDepth->Set(width, height);
}





__attribute__ (( visibility( "default" ) )) extern "C" void wasmGraphics_Assert(bool cond, const char* loc, const char* msg) {
 Graphics::Internal::Assert(cond, loc, msg);
}

__attribute__ (( visibility( "default" ) )) extern "C" Graphics::Device* wasm_Graphics_Initialize(Graphics::fpRequest allocPtr, Graphics::fpRelease releasePtr) {
 Graphics::Dependencies platform;
 platform.Request = allocPtr;
 platform.Release = releasePtr;
 Graphics::Device* device = (Graphics::Device*)allocPtr(sizeof(Graphics::Device));
 return Graphics::Initialize(*device, platform);
}

__attribute__ (( visibility( "default" ) )) extern "C" void wasm_Graphics_Shutdown(Graphics::Device* device) {
 Graphics::Shutdown(*device);
}


/*
Game Memory Allocator:

	Game Allocator is a generic memory manager intended for games, embedded devices, and web assembly.
	Given a large array of memory, the library provides functions to allocate and release that memory similar to malloc / free.
	The memory will be broken up into pages (4 KiB by default) and tracked at the page granularity. 
	A sub-allocator provided which breaks the page up into a fast free list for smaller allocation.

Usage:

	Let's assume you have a void* to some large area of memory and know how many bytes large that area is.

	Call the Memory::Initialize function to create an allocator. The first two arguments are the memory and size, 
	the third argument is the page size with which the memory should be managed. The default page size is 4 KiB

	The memory being passed it should be 8 byte aligned, and the size of the memory should be a multiple of pageSize.
	The Memory::AlignAndTrim helper function will align a region of memory so it's ready for initialize.
	It modifies the memory and size variables that are passed to the function. AlignAndTrim returns the number of bytes lost.

	Allocate memory with the allocator objects Allocate function, and release memory with the its Release function. 
	Allocate takes an optional alignment, which by default is 0. Only unaligned allocations utilize a fast free list allocator.
	Both functions also take a const char* which is optionally the location of the allocation.

	New and delete functions are also provided, these will invoke the constructor / destructor of the class they are
	being invoked on. New will forward up to three arguments and takes an optional location pointer.

	When you are finished with an allocator, clean it up by calling Memory::Shutdown. The shutdown function 
	will assert in debug builds if there are any memory leaks.

Example:

	void run() {
		// Declare how much memory to use
		// Adding (DefaultPageSize - 1) to size ensures that there is enough space for padding
		unsigned int size = MB(512) + (DefaultPageSize - 1); 

		// Allocate memory from the operating system
		LPVOID memory = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); // Windows

		// Initialize the global allocator
		u32 lost = Memory::AlignAndTrim(&m, &size, Memory::DefaultPageSize);
		Memory::Allocator* allocator = Memory::Initialize(m, size, Memory::DefaultPageSize);

		// Allocate & release memory
		int* number = allocator->Allocate(sizeof(int)); // Only the number of bytes is required
		allocator->Release(number); // Only the void* is required

		// New and delete can also be used:
		SomeClass* obj = allocator->New<SomeClass>("arguments");
		allocator->Delete(obj);

		// Cleanup the global allocator
		Memory::Shutdown(Memory::GlobalAllocator);
		Memory::GlobalAllocator = 0;

		// Release memory back to operating system
		VirtualFree(memory, 0, MEM_RELEASE);
	}

Compile flags:

	MEM_FIRST_FIT         -> This affects how fast memory is allocated. If it's set then every allocation
	                         searches for the first available page from the start of the memory. If it's not
					         set, then an allocation header is maintained. It's advanced with each allocation,
					         and new allocations search for memory from the allocation header.
	MEM_CLEAR_ON_ALLOC    -> When set, memory will be cleared to 0 before being returned from Memory::Allocate
	                         If both clear and debug on alloc are set, clear will take precedence
	MEM_DEBUG_ON_ALLOC    -> If set, full page allocations will fill the padding of the page with "-MEMORY"
	MEM_USE_SUBALLOCATORS -> If set, small allocations will be made using a free list allocaotr. There are free list
	                         allocators for 64, 128, 256, 512, 1024 and 2049 byte allocations. Only allocations that
							 don't specify an alignment can use the fast free list allocator. The sub-allocator will
							 provide better page utilization, for example a 4096 KiB page can hold 32 128 bit allocations.
	MEM_TRACK_LOCATION    -> If set, a const char* will be added to Memory::Allocation which tracks the __LINE__ and __FILE__
	                         of each allocation. Setting this bit will add 8 bytes to the Memory::Allocation struct.

Debugging:

	There are a few debug functions exposed in the Memory::Debug namespace. When an allocator is initialized, the page
	immediateley before the first allocatable page is reserved as a debug page. You can fill this page with whatever 
	data is needed. Any function in Memory::Debug might overwrite the contents of the debug page. You can get a pointer
	to the debug page of an allocator with the RequestDbgPage function. Be sure to release the page after you are dont
	using it by calling ReleaseDbgPage();

	The Memory::Debug::MemInfo function can be used to retrieve information about the state of the memory allocator.
	It provides meta data like how many pages are in use, a list of active allocations, and a visual bitmap chart to
	make debugging the memory bitmask easy. You can write this information to a file like so:

	DeleteFile(L"MemInfo.txt");
	HANDLE hFile = CreateFile(L"MemInfo.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	Memory::Debug::MemInfo(Memory::GlobalAllocator, [](const u8* mem, u32 size, void* fileHandle) {
		HANDLE file = *(HANDLE*)fileHandle;
		DWORD bytesWritten;
		WriteFile(file, mem, size, &bytesWritten, nullptr);
	}, &hFile);
	CloseHandle(hFile);
	
	There is a similar Memory::Debug::PageContent, which given a page number will dump the binary conent of a page.

Resources:

	Compile without CRT 
		https://yal.cc/cpp-a-very-tiny-dll/

	Ready Set Allocate:
		https://web.archive.org/web/20120419125628/http://www.altdevblogaday.com/2011/04/11/ready-set-allocate-part-1/
		https://web.archive.org/web/20120419125404/http://www.altdevblogaday.com/2011/04/26/ready-set-allocate-part-2/
		https://web.archive.org/web/20120419010208/http://www.altdevblogaday.com/2011/05/15/ready-set-allocate-part-3/
		https://web.archive.org/web/20120418212016/http://www.altdevblogaday.com/2011/05/26/ready-set-allocate-part-4/
		https://web.archive.org/web/20120413201435/http://www.altdevblogaday.com/2011/06/08/ready-set-allocate-part-5/
		https://web.archive.org/web/20120321205231/http://www.altdevblogaday.com/2011/06/30/ready-set-allocate-part-6/

	How to combine __LINE__ and __FILE__ into a c string:
		https://stackoverflow.com/questions/2653214/stringification-of-a-macro-value

	C++ overload new, new[], delete, and delete[]
		https://cplusplus.com/reference/new/operator%20new/
		https://cplusplus.com/reference/new/operator%20delete/
		https://cplusplus.com/reference/new/operator%20new[]/
		https://cplusplus.com/reference/new/operator%20delete[]/

	Memory alignment discussion:
		https://stackoverflow.com/questions/227897/how-to-allocate-aligned-memory-only-using-the-standard-library

	Scott Schurr's const string
		https://www.youtube.com/watch?v=BUnNA2dLRsU
*/

#pragma warning(disable:28251)

// When allocating new memory, if MEM_FIRST_FIT is defined and set to 1 every allocation will scan
// the available memory from the first bit to the last bit looking for enough space to satisfy the
// allocation. If MEM_FIRST_FIT is set to 0, then the memory is searched iterativley. Ie, when we 
// allocate the position in memory after the allocation is saved, and the next allocation starts
// searching from there.


// If set to 1, the allocator will clear or fill memory when allocating it



// Disables sub-allocators if defined


// If true, adds char* to each allocation




 typedef unsigned char u8;
 static_assert (sizeof(u8) == 1, "u8 should be defined as a 1 byte type");
 typedef unsigned long long u64;
 static_assert (sizeof(u64) == 8, "u64 should be defined as an 8 byte type");




 typedef long long i64;
 static_assert (sizeof(i64) == 8, "i64 should be defined as an 8 byte type");
 // _WASM32





 namespace Memory {
  typedef unsigned long ptr_type;
  typedef long diff_type;
  static_assert (sizeof(ptr_type) == 4, "ptr_type should be defined as a 4 byte type on a 32 bit system");
  static_assert (sizeof(diff_type) == 4, "diff_type should be defined as a 4 byte type on a 32 bit system");
 }




inline void* operator new (Memory::ptr_type n, void* ptr) {
 return ptr;
};

namespace Memory {
 // The callback allocator can be used to register a callback with each allocator. It's the same callback signature for both Allocate and Release
 typedef void (*Callback)(struct Allocator* allocator, void* allocationHeaderAddress, u32 bytesRequested, u32 bytesServed, u32 firstPage, u32 numPages);

 // Allocation struct uses a 32 bit offset instead of a pointer. This makes the maximum amount of memory GameAllocator can manage be 4 GiB
 typedef u32 Offset32;

 struct Allocation {

  const char* location;

  u32 padding_32bit; // Keep sizeof(Allocation) consistent between x64 & x86


  Offset32 prevOffset; // Offsets are the number of bytes from allocator
  Offset32 nextOffset;
  u32 size; // Unpadded allocation size, ie what you pass to malloc
  u32 alignment;
 };

 // Unlike Allocation, Allocator uses pointers. There is only ever one allocator
 // and saving a few bytes here isn't that important. Similarly, the free list
 // pointers exist even if MEM_USE_SUBALLOCATORS is off. This is done to keep the
 // size of this struct consistent for debugging.
 struct Allocator {
  Callback allocateCallback; // Callback for malloc / new
  Callback releaseCallback; // Callback for free / delete

  Allocation* free_64; // The max size for each of these lists is whatever the number after the
  Allocation* free_128; // underscore is, minus the size of the Allocation structure, which is 
  Allocation* free_256; // either 16 or 24 bytes (depenging if the location is tracked or not).
  Allocation* free_512; // For exampe, the largest allocation the 64 byte free list can hold is 50 bytes
  Allocation* free_1024; // There isn't much significance to these numbers, tune them to better match your structs
  Allocation* free_2048; // Only unaligned allocations (alignment of 0) can utilize the sub allocators.

  Allocation* active; // Memory that has been allocated, but not released

  u32 size; // In bytes, how much total memory is the allocator managing
  u32 requested; // How many bytes where requested (raw)
  u32 pageSize; // Default is 4096, but each allocator can have a unique size
  u32 scanBit; // Only used if MEM_FIRST_FIT is off

  u32 numPagesUsed;
  u32 peekPagesUsed; // Use this to monitor how much memory your application actually needs
  u32 mask;
  u32 mask_padding;


  u32 padding_32bit[9]; // Padding to make sure the struct stays the same size in x64 / x86 builds


  void* Allocate(u32 bytes, u32 alignemnt = 0, const char* location = 0);
  void Release(void* t, const char* location = 0);

  u8* RequestDbgPage();
  void ReleaseDbgPage();

  template<class T, typename A1>
  inline T* New(A1&& a1, const char* location = 0) {
   const u32 bytes = sizeof(T);
   const u32 alignment = 0;
   void* memory = this->Allocate(bytes, alignment, location);
   T* object = ::new (memory) T(a1);
   return object;
  }

  template<class T, typename A1, typename A2>
  inline T* New(A1&& a1, A2&& a2, const char* location = 0) {
   const u32 bytes = sizeof(T);
   const u32 alignment = 0;
   void* memory = this->Allocate(bytes, alignment, location);
   T* object = ::new (memory) T(a1, a2);
   return object;
  }

  template<class T, typename A1, typename A2, typename A3>
  inline T* New(A1&& a1, A2&& a2, A3&& a3, const char* location = 0) {
   const u32 bytes = sizeof(T);
   const u32 alignment = 0;
   void* memory = this->Allocate(bytes, alignment, location);
   T* object = ::new (memory) T(a1, a2, a3);
   return object;
  }

  template<class T>
  inline T* New(const char* location = 0) {
   const u32 bytes = sizeof(T);
   const u32 alignment = 0;
   void* memory = this->Allocate(bytes, alignment, location);
   T* object = ::new (memory) T();
   return object;
  }

  template<class T>
  inline void Delete(T* ptr, const char* location = 0) {
   T* obj = (T*)ptr;
   obj->T::~T();
   this->Release(ptr, location);
  }
 };

 // 4 KiB is a good default page size. Most of your small allocations will go trough the sub-allocators
 // so this page size is mostly important for larger allocations. Feel free to change to something more
 // appropriate if needed.
 const u32 DefaultPageSize = 4096;

 // Don't change tracking unit size. The bitmask that tracks which pages are free is stored as an
 // array of 32 bit integers. Changing this number would require changing how mem.cpp is implemented
 const u32 TrackingUnitSize = 32;
 // Don't change allocator alignment. Every allocator should start at an 8 byte aligned memory address.
 // Internally the allocator uses offsets to access some data, the start alignment is important.
 const u32 AllocatorAlignment = 8; // Should stay 8, even on 32 bit platforms

 // Call AlignAndTrim before Initialize to make sure that memory is aligned to alignment
 // and to make sure that the size of the memory (after it's been aligned) is a multiple of pageSize
 // both arguments are modified, the return value is how many bytes where removed
 u32 AlignAndTrim(void** memory, u32* size, u32 alignment = AllocatorAlignment, u32 pageSize = DefaultPageSize);

 // The initialize function will place the Allocator struct at the start of the provided memory. 
 // The allocaotr struct is followed by a bitmask, in which each bit tracks if a page is in use or not.
 // The bitmask is a bit u32 array. If the end of the bitmask is in the middle of a page, the rest of that
 // page is lost as padding. The next page is a debug page that you can use for anything, only functions in
 // the Memory::Debug namespace mess with the debug page, anything in Memory:: doesn't touch it.
 // The allocator that's returned should be used to set the global allocator.
 Allocator* Initialize(void* memory, u32 bytes, u32 pageSize = DefaultPageSize);

 // After you are finished with an allocator, shut it down. The shutdown function will assert in a debug build
 // if you have any memory that was allocated but not released. This function doesn't do much, it exists
 // to provide a bunch of asserts that ensure that an application is shutting down cleanly.
 void Shutdown(Allocator* allocator);

 // Memset and Memcpy utility functions. One big difference is that this set function only takes a u8.
 // both of these functions work on larger data types, then work their way down. IE: they try to set or
 // copy the memory using u64's, then u32's, then u16's, and finally u8's
 void* Set(void* memory, u8 value, u32 size, const char* location = 0);
 void Copy(void* dest, const void* source, u32 size, const char* location = 0);

 // The debug namespace let's you access information about the current state of the allocator,
 // gives you access to the contents of a page for debugging, and contains a debug page that
 // you can use for whatever. Be careful tough, MemInfo and PageContent might write to the
 // debug page, invalidating what was previously in there.
 namespace Debug {
  typedef void (*WriteCallback)(const u8* mem, u32 size, void* userdata);

  void MemInfo(Allocator* allocator, WriteCallback callback, void* userdata = 0);
  void PageContent(Allocator* allocator, u32 page, WriteCallback callback, void* userdata = 0);
 }
}



// Some compile time asserts to make sure that all our memory is sized correctly and aligns well
static_assert (sizeof(Memory::Allocator) % 8 == 0, "Memory::Allocator size needs to be 8 byte alignable for the allocation mask to start on u64 alignment without any padding");
static_assert (Memory::TrackingUnitSize% Memory::AllocatorAlignment == 0, "Memory::MaskTrackerSize must be a multiple of 8 (bits / byte)");
static_assert (sizeof(Memory::Allocator) == 96 + 8, "Memory::Allocator is not the expected size");

 static_assert (sizeof(Memory::Allocation) == 24, "Memory::Allocation should be 24 bytes (192 bits)");




// Use the __LOCATION__ macro to pack both __LINE__ and __FILE__ into a c string




// Make sure hte platform is set






#pragma warning(default:28251)

#pragma warning(disable:6011)
#pragma warning(disable:28182)



 typedef unsigned short u16;
 static_assert (sizeof(u16) == 2, "u16 should be defined as a 2 byte type");
namespace Memory {
 Allocator* wasmGlobalAllocator = 0;
}


extern "C" void* __cdecl memset(void* _mem, i32 _value, Memory::ptr_type _size) {
 return Memory::Set(_mem, (u8)_value, (u32)_size, "internal - memset");
}
extern "C" void wasmGraphics_Log(const char* loc, int locLen, const char* msg, int msgLen);

namespace Memory {
 namespace Debug {
  u32 u32toa(u8* dest, u32 destSize, u32 num);
 }
 static void Assert(bool condition, const char* msg, u32 line, const char* file) {

  if (condition == false) {
            u32 msg_len = 0;
            for (const char* i = msg; msg != 0 && *i != '\0'; ++i, ++msg_len);
   u32 loc_len = 0;
            for (const char* i = file; file != 0 && *i != '\0'; ++i, ++loc_len);
            wasmGraphics_Log(file, loc_len, msg, msg_len);
            __builtin_trap();
  }






 }

 static inline u32 AllocatorPaddedSize() {
  static_assert (sizeof(Memory::Allocator) % AllocatorAlignment == 0, "Memory::Allocator size needs to be 8 byte aligned for the allocation mask to start on this alignment without any padding");
  return sizeof(Allocator);
 }

 static inline u8* AllocatorPageMask(Allocator* allocator) {
  static_assert (sizeof(Memory::Allocator) % AllocatorAlignment == 0, "Memory::Allocator size needs to be 8 byte aligned for the allocation mask to start on this alignment without any padding");
  return ((u8*)allocator) + sizeof(Allocator);
 }

 static inline u32 AllocatorPageMaskSize(Allocator* allocator) { // This is the number of u8's that make up the AllocatorPageMask array
  const u32 allocatorNumberOfPages = allocator->size / allocator->pageSize; // 1 page = (probably) 4096 bytes, how many are needed
  Memory::Assert(allocator->size % allocator->pageSize == 0, "Allocator size should line up with page size", 62, "./mem.cpp");
  // allocatorNumberOfPages is the number of bits that are required to track memory

  // Pad out to sizeof(32) (if MaskTrackerSize is 32). This is because AllocatorPageMask will often be used as a u32 array
  // and we want to make sure that enough space is reserved.
  const u32 allocatorPageArraySize = allocatorNumberOfPages / TrackingUnitSize + (allocatorNumberOfPages % TrackingUnitSize ? 1 : 0);
  return allocatorPageArraySize * (TrackingUnitSize / 8); // In bytes, not bits
 }

 static inline void RemoveFromList(Allocator* allocator, Allocation** list, Allocation* allocation) {
  u32 allocationOffset = (u32)((u8*)allocation - (u8*)allocator);
  u32 listOffset = (u32)((u8*)(*list) - (u8*)allocator);

  Allocation* head = *list;

  if (head == allocation) { // Removing head
   if (head->nextOffset != 0) { // There is a next
    Allocation* allocNext = 0;
    if (allocation->nextOffset != 0) {
     allocNext = (Allocation*)((u8*)allocator + allocation->nextOffset);
    }
    Allocation* headerNext = 0;
    if (head->nextOffset != 0) {
     headerNext = (Allocation*)((u8*)allocator + head->nextOffset);
    }
    Memory::Assert(allocNext == headerNext, "On line: " "87" ", in file: " "./mem.cpp", 87, "./mem.cpp");
    Memory::Assert(headerNext->prevOffset == allocationOffset, "On line: " "88" ", in file: " "./mem.cpp", 88, "./mem.cpp");
    headerNext->prevOffset = 0;
   }
   Allocation* next = 0;
   if (head != 0 && head->nextOffset != 0) {
    next = (Allocation*)((u8*)allocator + head->nextOffset);
   }
   *list = next;
  }
  else {
   if (allocation->nextOffset != 0) {
    Allocation* _next = (Allocation*)((u8*)allocator + allocation->nextOffset);
    Memory::Assert(_next->prevOffset == allocationOffset, "On line: " "100" ", in file: " "./mem.cpp", 100, "./mem.cpp");
    _next->prevOffset = allocation->prevOffset;
   }
   if (allocation->prevOffset != 0) {
    Allocation* _prev = (Allocation*)((u8*)allocator + allocation->prevOffset);
    Memory::Assert(_prev->nextOffset == allocationOffset, "On line: " "105" ", in file: " "./mem.cpp", 105, "./mem.cpp");
    _prev->nextOffset = allocation->nextOffset;
   }
  }

  allocation->prevOffset = 0;
  allocation->nextOffset = 0;
 }

 static inline void AddtoList(Allocator* allocator, Allocation** list, Allocation* allocation) {
  u32 allocationOffset = (u32)((u8*)allocation - (u8*)allocator);
  u32 listOffset = (u32)((u8*)(*list) - (u8*)allocator);
  Allocation* head = *list;

  allocation->prevOffset = 0;
  allocation->nextOffset = 0;
  if (head != 0) {
   allocation->nextOffset = listOffset;
   head->prevOffset = allocationOffset;
  }
  *list = allocation;
 }

 // Returns 0 on error. Since the first page is always tracking overhead it's invalid for a range
 static inline u32 FindRange(Allocator* allocator, u32 numPages, u32 searchStartBit) {
  Memory::Assert(allocator != 0, "On line: " "130" ", in file: " "./mem.cpp", 130, "./mem.cpp");
  Memory::Assert(numPages != 0, "On line: " "131" ", in file: " "./mem.cpp", 131, "./mem.cpp");

  u32 * mask = (u32*)AllocatorPageMask(allocator);
  u32 numBitsInMask = AllocatorPageMaskSize(allocator) * 8;
  u32 numElementsInMask = AllocatorPageMaskSize(allocator) / (TrackingUnitSize / 8);
  Memory::Assert(allocator->size % allocator->pageSize == 0, "Memory::FindRange, the allocators size must be a multiple of Memory::PageSize, otherwise there would be a partial page at the end", 136, "./mem.cpp");
  Memory::Assert(mask != 0, "On line: " "137" ", in file: " "./mem.cpp", 137, "./mem.cpp");
  Memory::Assert(numBitsInMask != 0, "On line: " "138" ", in file: " "./mem.cpp", 138, "./mem.cpp");

  u32 startBit = 0;
  u32 numBits = 0;

  for (u32 i = searchStartBit; i < numBitsInMask; ++i) {
   u32 m = i / TrackingUnitSize;
   u32 b = i % TrackingUnitSize;

   Memory::Assert(m < numElementsInMask, "indexing mask out of range", 147, "./mem.cpp");
   bool set = mask[m] & (1 << b);

   if (!set) {
    if (startBit == 0) {
     startBit = i;
     numBits = 1;
    }
    else {
     numBits++;
    }
   }
   else {
    startBit = 0;
    numBits = 0;
   }

   if (numBits == numPages) {
    break;
   }
  }

  if (numBits != numPages || startBit == 0) {
   startBit = 0;
   numBits = 0;

   for (u32 i = 0; i < searchStartBit; ++i) {
    u32 m = i / TrackingUnitSize;
    u32 b = i % TrackingUnitSize;

    bool set = mask[m] & (1 << b);

    if (!set) {
     if (startBit == 0) {
      startBit = i;
      numBits = 1;
     }
     else {
      numBits++;
     }
    }
    else {
     startBit = 0;
     numBits = 0;
    }

    if (numBits == numPages) {
     break;
    }
   }
  }

  allocator->scanBit = startBit + numPages;

  Memory::Assert(numBits == numPages, "Memory::FindRange Could not find enough memory to fufill request", 201, "./mem.cpp");
  Memory::Assert(startBit != 0, "Memory::FindRange Could not memory fufill request", 202, "./mem.cpp");
  if (numBits != numPages || startBit == 0 || allocator->size % allocator->pageSize != 0) {
   Memory::Assert(false, "On line: " "204" ", in file: " "./mem.cpp", 204, "./mem.cpp");
   return 0;
  }

  return startBit;
 }

 static inline void SetRange(Allocator* allocator, u32 startBit, u32 bitCount) {
  Memory::Assert(allocator != 0, "On line: " "212" ", in file: " "./mem.cpp", 212, "./mem.cpp");
  Memory::Assert(bitCount != 0, "On line: " "213" ", in file: " "./mem.cpp", 213, "./mem.cpp");

  u32* mask = (u32*)AllocatorPageMask(allocator);
  Memory::Assert(allocator->size % allocator->pageSize == 0, "Memory::FindRange, the allocators size must be a multiple of Memory::PageSize, otherwise there would be a partial page at the end", 216, "./mem.cpp");
  Memory::Assert(mask != 0, "On line: " "217" ", in file: " "./mem.cpp", 217, "./mem.cpp");


  u32 numBitsInMask = AllocatorPageMaskSize(allocator) * 8;
  Memory::Assert(numBitsInMask != 0, "On line: " "221" ", in file: " "./mem.cpp", 221, "./mem.cpp");

  u32 numElementsInMask = AllocatorPageMaskSize(allocator) / (TrackingUnitSize / 8);

  for (u32 i = startBit; i < startBit + bitCount; ++i) {

   u32 m = i / TrackingUnitSize;
   u32 b = i % TrackingUnitSize;

   Memory::Assert(m < numElementsInMask, "indexing mask out of range", 230, "./mem.cpp");

   Memory::Assert(i < numBitsInMask, "On line: " "232" ", in file: " "./mem.cpp", 232, "./mem.cpp");
   bool set = mask[m] & (1 << b);
   Memory::Assert(!set, "On line: " "234" ", in file: " "./mem.cpp", 234, "./mem.cpp");


   mask[m] |= (1 << b);
  }

  Memory::Assert(allocator->numPagesUsed <= numBitsInMask, "Memory::FindRange, over allocating", 240, "./mem.cpp");
  Memory::Assert(allocator->numPagesUsed + bitCount <= numBitsInMask, "Memory::FindRange, over allocating", 241, "./mem.cpp");
  allocator->numPagesUsed += bitCount;
  if (allocator->numPagesUsed > allocator->peekPagesUsed) {
   allocator->peekPagesUsed = allocator->numPagesUsed;
  }
 }

 static inline void ClearRange(Allocator* allocator, u32 startBit, u32 bitCount) {
  Memory::Assert(allocator != 0, "On line: " "249" ", in file: " "./mem.cpp", 249, "./mem.cpp");
  Memory::Assert(bitCount != 0, "On line: " "250" ", in file: " "./mem.cpp", 250, "./mem.cpp");

  u32* mask = (u32*)AllocatorPageMask(allocator);
  Memory::Assert(allocator->size % allocator->pageSize == 0, "Memory::FindRange, the allocators size must be a multiple of Memory::PageSize, otherwise there would be a partial page at the end", 253, "./mem.cpp");
  Memory::Assert(mask != 0, "On line: " "254" ", in file: " "./mem.cpp", 254, "./mem.cpp");


  u32 numBitsInMask = AllocatorPageMaskSize(allocator) * 8;
  Memory::Assert(numBitsInMask != 0, "On line: " "258" ", in file: " "./mem.cpp", 258, "./mem.cpp");


  u32 numElementsInMask = AllocatorPageMaskSize(allocator) / (TrackingUnitSize / 8);

  for (u32 i = startBit; i < startBit + bitCount; ++i) {

   u32 m = i / TrackingUnitSize;
   u32 b = i % TrackingUnitSize;

   Memory::Assert(m < numElementsInMask, "indexing mask out of range", 268, "./mem.cpp");


   Memory::Assert(i < numBitsInMask, "On line: " "271" ", in file: " "./mem.cpp", 271, "./mem.cpp");
   bool set = mask[m] & (1 << b);
   Memory::Assert(set, "On line: " "273" ", in file: " "./mem.cpp", 273, "./mem.cpp");


   mask[m] &= ~(1 << b);
  }

  Memory::Assert(allocator->numPagesUsed != 0, "On line: " "279" ", in file: " "./mem.cpp", 279, "./mem.cpp");
  Memory::Assert(allocator->numPagesUsed >= bitCount != 0, "underflow", 280, "./mem.cpp");
  allocator->numPagesUsed -= bitCount;
 }


 // This function will chop the provided page into several blocks. Since the block size is constant, we
 // know that headers will be laid out at a stride of blockSize. There is no additional tracking needed.
 void* SubAllocate(u32 requestedBytes, u32 blockSize, Allocation** freeList, const char* location, Allocator* allocator) {
  Memory::Assert(blockSize < allocator->pageSize, "Block size must be less than page size", 288, "./mem.cpp");

  // There is no blocks of the requested size available. Reserve 1 page, and carve it up into blocks.
  bool grabNewPage = *freeList == 0;
  if (*freeList == 0) {
   // Find and reserve 1 free page

   const u32 page = FindRange(allocator, 1, 0);



   SetRange(allocator, page, 1);

   // Zero out the pages memory
   u8* mem = (u8*)allocator + allocator->pageSize * page;
   Set(mem, 0, allocator->pageSize, "On line: " "303" ", in file: " "./mem.cpp");

   // Figure out how many blocks fit into this page
   const u32 numBlocks = allocator->pageSize / blockSize;
   Memory::Assert(numBlocks > 0, "On line: " "307" ", in file: " "./mem.cpp", 307, "./mem.cpp");
   Memory::Assert(numBlocks < 128, "On line: " "308" ", in file: " "./mem.cpp", 308, "./mem.cpp");

   // For each block in this page, initialize it's header and add it to the free list
   for (u32 i = 0; i < numBlocks; ++i) {
    Allocation* alloc = (Allocation*)mem;
    mem += blockSize;

    // Initialize the allocation header
    alloc->prevOffset = 0;
    alloc->nextOffset = 0;
    alloc->size = 0;
    alloc->alignment = 0;

    alloc->location = location;


    AddtoList(allocator, freeList, alloc);
   }
  }
  Memory::Assert(*freeList != 0, "The free list literally can't be zero here...", 327, "./mem.cpp");

  // At this point we know the free list has some number of blocks in it. 
  // Save a reference to the current header & advance the free list
  // Advance the free list, we're going to be using this one.
  Allocation* block = *freeList;
  if ((*freeList)->nextOffset != 0) { // Advance one
   Allocation* _next = (Allocation*)((u8*)allocator + (*freeList)->nextOffset);
   _next->prevOffset = 0;
   *freeList = (Allocation*)((u8*)allocator + (*freeList)->nextOffset); // freeList = freeList.next
  }
  else {
   *freeList = 0;
  }

  block->prevOffset = 0;
  block->size = requestedBytes;
  block->alignment = 0;

  block->location = location;


  AddtoList(allocator, &allocator->active, block); // Sets block->next

  if (allocator->allocateCallback != 0) {
   u32 firstPage = ((u32)((u8*)block - (u8*)allocator)) / allocator->pageSize;
   allocator->allocateCallback(allocator, block, requestedBytes, blockSize, firstPage, grabNewPage? 1 : 0);
  }

  // Memory always follows the header
  return (u8*)block + sizeof(Allocation);
 }



 void SubRelease(void* memory, u32 blockSize, Allocation** freeList, const char* location, Allocator* allocator) {
  // Find the allocation header and mark it as free. Early out on double free to avoid breaking.
  Allocation* header = (Allocation*)((u8*)memory - sizeof(Allocation));
  Memory::Assert(header->size != 0, "Double Free!", 377, "./mem.cpp"); // Make sure it's not a double free
  if (header->size == 0) {
   Memory::Assert(false, "On line: " "379" ", in file: " "./mem.cpp", 379, "./mem.cpp");
   return;
  }
  u32 oldSize = header->size;
  header->size = 0;

  // Now remove from the active list.
  RemoveFromList(allocator, &allocator->active, header);
  // Add memory back into the free list
  AddtoList(allocator, freeList, header);

  header->location = "SubRelease released this block";


  // Find the first allocation inside the page
  u32 startPage = (u32)((u8*)header - (u8*)allocator) / allocator->pageSize;

  u8* mem =(u8*)allocator + startPage * allocator->pageSize;

  // Each sub allocator page contains multiple blocks. check if all of the blocks 
  // belonging to a single page are free, if they are, release the page.
  bool releasePage = true;

  const u32 numAllocationsPerPage = allocator->pageSize / blockSize;
  Memory::Assert(numAllocationsPerPage >= 1, "On line: " "403" ", in file: " "./mem.cpp", 403, "./mem.cpp");
  for (u32 i = 0; i < numAllocationsPerPage; ++i) {
   Allocation* alloc = (Allocation*)mem;
   if (alloc->size > 0) {
    releasePage = false;
    break;
   }
   mem += blockSize;
  }

  // If appropriate, release entire page
  if (releasePage) {
   // Remove from free list
   mem = (u8*)allocator + startPage * allocator->pageSize;
   for (u32 i = 0; i < numAllocationsPerPage; ++i) {
    Allocation* iter = (Allocation*)mem;
    mem += blockSize;
    Memory::Assert(iter != 0, "On line: " "420" ", in file: " "./mem.cpp", 420, "./mem.cpp");

    RemoveFromList(allocator, freeList, iter);
   }

   // Clear the tracking bits
   Memory::Assert(startPage > 0, "On line: " "426" ", in file: " "./mem.cpp", 426, "./mem.cpp");
   ClearRange(allocator, startPage, 1);
  }

  if (allocator->releaseCallback != 0) {
   allocator->releaseCallback(allocator, header, oldSize, blockSize, startPage, releasePage ? 1 : 0);
  }
 }

} // Namespace Memory




 extern unsigned char __heap_base;
 extern unsigned char __data_end;

 // These are wasm shim functions

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmHeapSize(int memSize) {
  void* heapPtr = &__heap_base;

  Memory::ptr_type heapAddr = (Memory::ptr_type)heapPtr;
  Memory::ptr_type maxAddr = (Memory::ptr_type)memSize;

  Memory::ptr_type heapSize = maxAddr - heapAddr;
  return (int)heapSize;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" Memory::Allocator* GameAllocator_wasmInitialize(int heapSize) {
  void* memory = &__heap_base;
  u32 size = (u32)heapSize; //GameAllocator_wasmHeapSize(totalMemorySize);

  Memory::AlignAndTrim(&memory, &size);
  Memory::Allocator* allocator = Memory::Initialize(memory, size);
  Memory::wasmGlobalAllocator = allocator;

  return allocator;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" void GameAllocator_wasmShutdown(Memory::Allocator* allocator) {
  Memory::Shutdown(allocator);
 }

 __attribute__ (( visibility( "default" ) )) extern "C" void* GameAllocator_wasmAllocate(Memory::Allocator* allocator, int bytes, int alignment) {
  return Memory::wasmGlobalAllocator->Allocate(bytes, alignment, "GameAllocator_wasmAllocate");
 }

 __attribute__ (( visibility( "default" ) )) extern "C" void GameAllocator_wasmRelease(Memory::Allocator* allocator, void* mem) {
  Memory::wasmGlobalAllocator->Release(mem, "GameAllocator_wasmAllocate");
 }

 __attribute__ (( visibility( "default" ) )) extern "C" void GameAllocator_wasmSet(void* mem, int val, int size) {
  Memory::Set(mem, (u8)val, (u32)size, "GameAllocator_wasmAllocate");
 }

 __attribute__ (( visibility( "default" ) )) extern "C" void GameAllocator_wasmCopy(void* dst, const void* src, int size) {
  Memory::Copy(dst, src, (u32)size, "GameAllocator_wasmAllocate");
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetNumPages(Memory::Allocator* a) {
        return a->size / a->pageSize;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetNumPagesInUse(Memory::Allocator* a) {
        return a->numPagesUsed;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetPeekPagesUsed(Memory::Allocator* a) {
        return a->peekPagesUsed;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetRequestedBytes(Memory::Allocator* a) {
        return a->requested;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetServedBytes(Memory::Allocator* a) {
  u32 maskSize = AllocatorPageMaskSize(a) / (sizeof(u32) / sizeof(u8)); // convert from u8 to u32
  u32 metaDataSizeBytes = sizeof(Memory::Allocator) + (maskSize * sizeof(u32));
  u32 numberOfMasksUsed = metaDataSizeBytes / a->pageSize;
  if (metaDataSizeBytes % a->pageSize != 0) {
   numberOfMasksUsed += 1;
  }
  metaDataSizeBytes = numberOfMasksUsed * a->pageSize; // This way, allocatable will start on a page boundary
  // Account for meta data
  metaDataSizeBytes += a->pageSize;
  numberOfMasksUsed += 1;

  u32 numPages = a->size / a->pageSize;
  u32 usedPages = a->numPagesUsed;
  u32 freePages = numPages - usedPages;
  u32 overheadPages = metaDataSizeBytes / a->pageSize;

  return (usedPages - overheadPages) * a->pageSize;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmIsPageInUse(Memory::Allocator* a, int page) {
  u32 m = page / Memory::TrackingUnitSize;
  u32 b = page % Memory::TrackingUnitSize;
  u32 * mask = (u32*)Memory::AllocatorPageMask(a);

  bool set = mask[m] & (1 << b);
  return set;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetSize(Memory::Allocator* a) {
        return a->size;
 }

 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmGetNumOverheadPages(Memory::Allocator* a) {
  u32 maskSize = Memory::AllocatorPageMaskSize(a) / (sizeof(u32) / sizeof(u8)); // convert from u8 to u32
  u32 metaDataSizeBytes = Memory::AllocatorPaddedSize() + (maskSize * sizeof(u32));
  u32 numberOfMasksUsed = metaDataSizeBytes / a->pageSize;
  if (metaDataSizeBytes % a->pageSize != 0) {
   numberOfMasksUsed += 1;
  }
  metaDataSizeBytes = numberOfMasksUsed * a->pageSize; // This way, allocatable will start on a page boundary
  // Account for meta data
  metaDataSizeBytes += a->pageSize;
  numberOfMasksUsed += 1;

  u32 overheadPages = metaDataSizeBytes / a->pageSize;

  return (int)overheadPages;
 }

 // Helper functions
 __attribute__ (( visibility( "default" ) )) extern "C" int GameAllocator_wasmStrLen(const char* str) {
  if (str == 0) {
   return 0;
  }

  const char *s = str;
  while (*s) {
   ++s;
  }
  return (s - str);
 }

    extern "C" void GameAllocator_jsBuildMemState(const u8* msg, int len);

 __attribute__ (( visibility( "default" ) )) extern "C" void GameAllocator_wasmDumpState(Memory::Allocator* allocator) {
  Memory::Debug::MemInfo(allocator, [](const u8* mem, u32 size, void* userdata) {
   GameAllocator_jsBuildMemState(mem, (int)size);
  }, 0);
 }

 __attribute__ (( visibility( "default" ) )) extern "C" void* GameAllocator_wasmGetAllocationDebugName(Memory::Allocator* allocator, void* _m) {
  const char* l = "mem_GetAllocationDebugName";

  u8* debugPage = allocator->RequestDbgPage();
  u32 debugSize = allocator->pageSize;

  // Reset memory buffer
  Memory::Set(debugPage, 0, debugSize, l);
  u8* i_to_a_buff = debugPage; // Used to convert numbers to strings
  const u32 i_to_a_buff_size = GameAllocator_wasmStrLen((const char*)"18446744073709551615") + 1; // u64 max

  u8* mem = i_to_a_buff + i_to_a_buff_size;
  u32 memSize = allocator->pageSize - i_to_a_buff_size;

  u8* m = (u8*)_m - sizeof(Memory::Allocation);
  Memory::Allocation* iter = (Memory::Allocation*)m;

  Memory::Copy(mem, "Address: ", 9, l);
  mem += 9; memSize -= 9;

  u32 allocationOffset = (u32)((u8*)iter - (u8*)allocator);
  i32 i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, allocationOffset);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  Memory::Copy(mem, ", size: ", 8, l);
  mem += 8; memSize -= 8;

  i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, iter->size);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  Memory::Copy(mem, ", padded: ", 10, l);
  mem += 10; memSize -= 10;

  u32 alignment = iter->alignment;
  u32 allocationHeaderPadding = 0;
  if (alignment != 0) { // Add padding to the header to compensate for alignment
   allocationHeaderPadding = alignment - 1; // Somewhere in this range, we will be aligned
  }

  u32 realSize = iter->size + (u32)(sizeof(Memory::Allocation)) + allocationHeaderPadding;
  i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, realSize);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  Memory::Copy(mem, ", alignment: ", 13, l);
  mem += 13; memSize -= 13;

  i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, iter->alignment);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  Memory::Copy(mem, ", first page: ", 14, l);
  mem += 14; memSize -= 14;

  i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, (allocationOffset) / allocator->pageSize);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  Memory::Copy(mem, ", prev: ", 8, l);
  mem += 8; memSize -= 8;

  i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, iter->prevOffset);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  Memory::Copy(mem, ", next: ", 8, l);
  mem += 8; memSize -= 8;

  i_len = Memory::Debug::u32toa(i_to_a_buff, i_to_a_buff_size, iter->nextOffset);
  Memory::Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  u32 pathLen = 0;

  if (iter->location != 0) {
   pathLen = GameAllocator_wasmStrLen((const char*)iter->location);
  }


  Memory::Copy(mem, ", location: ", 12, l);
  mem += 12; memSize -= 12;


  if (iter->location == 0) {



   Memory::Copy(mem, "null", 4, l);
   mem += 4; memSize -= 4;
  }

  else {
   Memory::Copy(mem, iter->location, pathLen, l);
   mem += pathLen;
   memSize -= pathLen;
  }


  *mem = '\0';

  allocator->ReleaseDbgPage();

  return debugPage + i_to_a_buff_size;
 }



u32 Memory::AlignAndTrim(void** memory, u32* size, u32 alignment, u32 pageSize) {



 u32 ptr = (u32)((const void*)(*memory));



 u32 delta = 0;

    if (alignment != 0) {
        // Align to 8 byte boundary. This is so the mask array lines up on a u64
        u32 alignmentDelta = alignment - (u32)(ptr % alignment);
        Memory::Assert(alignmentDelta <= (*size), "On line: " "702" ", in file: " "./mem.cpp", 702, "./mem.cpp");
        if (alignmentDelta > *size) { // In release mode, we want to fail on asserts
            *memory = 0;
            *size = 0;
            return 0;
        }

        if (ptr % alignment != 0) {
            u8* mem = (u8*)(*memory);

            delta += alignmentDelta;
            mem += alignmentDelta;
            *size -= alignmentDelta;
            *memory = mem;
        }
    }

 // Trim to page size (4096) to make sure the provided memory can be chunked up perfectly
 if ((*size) % pageSize != 0) {
  u32 diff = (*size) % pageSize;
  Memory::Assert(*size >= diff, "On line: " "722" ", in file: " "./mem.cpp", 722, "./mem.cpp");
        if (*size < diff) { // In release mode, fail on assert
            *memory = 0;
            *size = 0;
            return 0;
        }
  *size -= diff;
  delta += diff;
 }

 return delta;
}

Memory::Allocator* Memory::Initialize(void* memory, u32 bytes, u32 pageSize) {
 Memory::Assert(pageSize % AllocatorAlignment == 0, "Memory::Initialize, Page boundaries are expected to be on 8 bytes", 736, "./mem.cpp");
 // First, make sure that the memory being passed in is aligned well



 u32 ptr = (u32)((const void*)memory);



 Memory::Assert(ptr % AllocatorAlignment == 0, "Memory::Initialize, Memory being managed should be 8 byte aligned. Consider using Memory::AlignAndTrim", 745, "./mem.cpp");
 Memory::Assert(bytes % pageSize == 0, "Memory::Initialize, the size of the memory being managed must be aligned to Memory::PageSize", 746, "./mem.cpp");
 Memory::Assert(bytes / pageSize >= 10, "Memory::Initialize, minimum memory size is 10 pages, page size is Memory::PageSize", 747, "./mem.cpp");

 // Set up the allocator
 Allocator* allocator = (Allocator*)memory;
 Set(memory, 0, sizeof(Allocator), "Memory::Initialize");
 allocator->size = bytes;
 allocator->pageSize = pageSize;
 allocator->mask = 0;

 // Set up the mask that will track our allocation data
 u32* mask = (u32*)AllocatorPageMask(allocator);
 u32 maskSize = AllocatorPageMaskSize(allocator) / (sizeof(u32) / sizeof(u8)); // convert from u8 to u32
 Set(mask, 0, sizeof(u32) * maskSize, "On line: " "759" ", in file: " "./mem.cpp");

 // Find how many pages the meta data for the header + allocation mask will take up. 
 // Store the offset to first allocatable, 
 u32 metaDataSizeBytes = AllocatorPaddedSize() + (maskSize * sizeof(u32));
 u32 numberOfMasksUsed = metaDataSizeBytes / pageSize;
 if (metaDataSizeBytes % pageSize != 0) {
  numberOfMasksUsed += 1;
 }
 metaDataSizeBytes = numberOfMasksUsed * pageSize; // This way, allocatable will start on a page boundary

 // Add a debug page at the end
 metaDataSizeBytes += pageSize;
 numberOfMasksUsed += 1;

 //allocator->offsetToAllocatable = metaDataSizeBytes;
 allocator->scanBit = 0;
 SetRange(allocator, 0, numberOfMasksUsed);
 allocator->requested = 0;

 if (ptr % AllocatorAlignment != 0 || bytes % pageSize != 0 || bytes / pageSize < 10) {
  Memory::Assert(false, "On line: " "780" ", in file: " "./mem.cpp", 780, "./mem.cpp");
  return 0;
 }

 return (Allocator*)memory;
}

void Memory::Shutdown(Allocator* allocator) {
 Memory::Assert(allocator != 0, "Memory::Shutdown called without it being initialized", 788, "./mem.cpp");
 u32* mask = (u32*)AllocatorPageMask(allocator);
 u32 maskSize = AllocatorPageMaskSize(allocator) / (sizeof(u32) / sizeof(u8)); // convert from u8 to u32
 Memory::Assert(allocator->size > 0, "Memory::Shutdown, trying to shut down an un-initialized allocator", 791, "./mem.cpp");

 // Unset tracking bits
 u32 metaDataSizeBytes = AllocatorPaddedSize() + (maskSize * sizeof(u32));
 u32 numberOfMasksUsed = metaDataSizeBytes / allocator->pageSize;
 if (metaDataSizeBytes % allocator->pageSize != 0) {
  numberOfMasksUsed += 1;
 }
 metaDataSizeBytes = numberOfMasksUsed * allocator->pageSize;

 // There is a debug between the memory bitmask and allocatable memory
 metaDataSizeBytes += allocator->pageSize;
 numberOfMasksUsed += 1;

 ClearRange(allocator, 0, numberOfMasksUsed);
 Memory::Assert(allocator->requested == 0, "Memory::Shutdown, not all memory has been released", 806, "./mem.cpp");

 Memory::Assert(allocator->active == 0, "There are active allocations in Memory::Shutdown, leaking memory", 808, "./mem.cpp");
 Memory::Assert(allocator->free_64 == 0, "Free list is not empty in Memory::Shutdown, leaking memory", 809, "./mem.cpp");
 Memory::Assert(allocator->free_128 == 0, "Free list is not empty in Memory::Shutdown, leaking memory", 810, "./mem.cpp");
 Memory::Assert(allocator->free_256 == 0, "Free list is not empty in Memory::Shutdown, leaking memory", 811, "./mem.cpp");
 Memory::Assert(allocator->free_512 == 0, "Free list is not empty in Memory::Shutdown, leaking memory", 812, "./mem.cpp");
 Memory::Assert(allocator->free_1024 == 0, "Free list is not empty in Memory::Shutdown, leaking memory", 813, "./mem.cpp");
 Memory::Assert(allocator->free_2048 == 0, "Free list is not empty in Memory::Shutdown, leaking memory", 814, "./mem.cpp");


 // In debug mode only, we will scan the entire mask to make sure all memory has been free-d
 for (u32 i = 0; i < maskSize; ++i) {
  Memory::Assert(mask[i] == 0, "Page tracking unit isn't empty in Memory::Shutdown, leaking memory.", 819, "./mem.cpp");
 }

}

void Memory::Copy(void* dest, const void* source, u32 size, const char* location) {





 u32 dst_ptr = (u32)((const void*)(dest));
 u32 src_ptr = (u32)((const void*)(source));
 u32 alignment = sizeof(u32);




 if (dst_ptr % alignment != 0 || src_ptr % alignment != 0) {
  // Memory is not aligned well, fall back on slow copy
  u8* dst = (u8*)dest;
  const u8* src = (const u8*)source;
  for (u32 i = 0; i < size; ++i) {
   dst[i] = src[i];
  }
  return;
 }
 u32 size_32 = size / sizeof(u32);
 u32* dst_32 = (u32*)dest;
 const u32* src_32 = (u32*)source;

 for (u32 i = 0; i < size_32; ++i) {
  dst_32[i] = src_32[i];
 }




 u32 size_16 = (size - size_32 * sizeof(u32)) / sizeof(u16);

 u16* dst_16 = (u16*)(dst_32 + size_32);
 const u16* src_16 = (const u16*)(src_32 + size_32);
 for (u32 i = 0; i < size_16; ++i) {
  dst_16[i] = src_16[i];
 }




 u32 size_8 = (size - size_32 * sizeof(u32) - size_16 * sizeof(u16));

 u8* dst_8 = (u8*)(dst_16 + size_16);
 const u8* src_8 = (const u8*)(src_16 + size_16);
 for (u32 i = 0; i < size_8; ++i) {
  dst_8[i] = src_8[i];
 }




 Memory::Assert(size_32 * sizeof(u32) + size_16 * sizeof(u16) + size_8 == size, "Number of pages not adding up", 894, "./mem.cpp");



}

// MSVC generates a recursive memset with this implementation. The naive one works fine.
//#pragma optimize( "", off )
void* Memory::Set(void* memory, u8 value, u32 size, const char* location) {
 if (memory == 0) {
  return 0; // Can't set null!
 }





 u32 ptr = (u32)((const void*)(memory));
 u32 alignment = sizeof(u32);




 if (size <= alignment) {
  u8* mem = (u8*)memory;
  /* MSCV was optimizing this loop into a recursive call?
		for (u32 i = 0; i < size; ++i) {
			mem[i] = value;
		}*/
  while ((alignment--) > 0) {
   *mem = value;
  }
  return memory;
 }

 // Algin memory if needed
 Memory::Assert(alignment >= (ptr % alignment), "On line: " "930" ", in file: " "./mem.cpp", 930, "./mem.cpp");
 u32 alignDelta = (u32)(alignment - (ptr % alignment));
 Memory::Assert(alignDelta <= alignment, "On line: " "932" ", in file: " "./mem.cpp", 932, "./mem.cpp");
 Memory::Assert(size >= alignDelta, "On line: " "933" ", in file: " "./mem.cpp", 933, "./mem.cpp");

 u8* mem = (u8*)(memory);
 if (alignDelta != 0) {
  if (alignDelta > size) {
   alignDelta = size;
  }
  for (u32 iter = 0; iter < alignDelta; ++iter) {
   mem[iter] = value;
  }

  mem += alignDelta;
  size -= alignDelta;
 }
 u32 size_32 = size / sizeof(u32);
 u32* ptr_32 = (u32*)memory;

 u32 val_32 = (((u32)value) << 8) | (((u32)value) << 16) | (((u32)value) << 24) | ((u32)value);
 for (u32 i = 0; i < size_32; ++i) {
  ptr_32[i] = val_32;
 }




 u32 size_16 = (size - size_32 * sizeof(u32)) / sizeof(u16);

 u16* ptr_16 = (u16*)(ptr_32 + size_32);
 u32 val_16 = (((u16)value) << 8) | ((u16)value);
 for (u32 i = 0; i < size_16; ++i) {
  ptr_16[i] = val_16;
 }




 u32 size_8 = (size - size_32 * sizeof(u32) - size_16 * sizeof(u16));

 u8* ptr_8 = (u8*)(ptr_16 + size_16);
 for (u32 i = 0; i < size_8; ++i) {
  ptr_8[i] = value;
 }




 Memory::Assert(size_32 * sizeof(u32) + size_16 * sizeof(u16) + size_8 == size, "Number of pages not adding up", 994, "./mem.cpp");




 return memory;
}
//#pragma optimize( "", on )

u8* Memory::Allocator::RequestDbgPage() {
 Memory::Allocator* allocator = this;

 Memory::Assert(allocator->mask == 0, "Debug page already in use", 1006, "./mem.cpp");
 allocator->mask = 1;

 // Set up the mask that will track our allocation data
 u32* mask = (u32*)AllocatorPageMask(allocator);
 u32 maskSize = AllocatorPageMaskSize(allocator) / (sizeof(u32) / sizeof(u8)); // convert from u8 to u32

 // Find how many pages the meta data for the header + allocation mask will take up. 
 // Store the offset to first allocatable, 
 u32 metaDataSizeBytes = AllocatorPaddedSize() + (maskSize * sizeof(u32));
 u32 numberOfMasksUsed = metaDataSizeBytes / allocator->pageSize;
 if (metaDataSizeBytes % allocator->pageSize != 0) {
  numberOfMasksUsed += 1;
 }
 metaDataSizeBytes = numberOfMasksUsed * allocator->pageSize; // This way, allocatable will start on a page boundary

 // Add a debug page at the end
 metaDataSizeBytes += allocator->pageSize;
 numberOfMasksUsed += 1;

 u8* debugPage = (u8*)allocator + metaDataSizeBytes - allocator->pageSize; // Debug page is always one page before allocatable
 return debugPage;
}

void Memory::Allocator::ReleaseDbgPage() {
 Memory::Allocator* allocator = this;

 Memory::Assert(allocator->mask != 0, "Debug page not in use", 1033, "./mem.cpp");
 allocator->mask = 0;
}

void* Memory::Allocator::Allocate(u32 bytes, u32 alignment, const char* location) {
 if (bytes == 0) {
  bytes = 1; // At least one byte required
 }
 Memory::Allocator* allocator = this;
 Memory::Assert(bytes < allocator->size, "Memory::Allocate trying to allocate more memory than is available", 1042, "./mem.cpp");
 Memory::Assert(bytes < allocator->size - allocator->requested, "Memory::Allocate trying to allocate more memory than is available", 1043, "./mem.cpp");

 u32 allocationHeaderPadding = 0;
 if (alignment != 0) { // Add paddnig to make sure we can align the memory
  allocationHeaderPadding = alignment - 1; // Somewhere in this range, we will be aligned
 }
 u32 allocationHeaderSize = sizeof(Allocation) + allocationHeaderPadding;

 // Add the header size to our allocation size
 u32 allocationSize = bytes; // Add enough space to pad out for alignment
 allocationSize += allocationHeaderSize;

 // Figure out how many pages are going to be needed to hold that much memory
 u32 numPagesRequested = allocationSize / allocator->pageSize + (allocationSize % allocator->pageSize ? 1 : 0);
 Memory::Assert(numPagesRequested > 0, "Memory::Allocate needs to request at least 1 page", 1057, "./mem.cpp");

 // We can record the request here. It's made before the allocation callback, and is valid for sub-allocations too.
 allocator->requested += bytes;
 Memory::Assert(allocator->requested < allocator->size, "On line: " "1061" ", in file: " "./mem.cpp", 1061, "./mem.cpp");


 if (alignment == 0) {
  if (allocationSize <= 64) {
   return SubAllocate(bytes, 64, &allocator->free_64, location, allocator);
  }
  else if (allocationSize <= 128) {
   return SubAllocate(bytes, 128, &allocator->free_128, location, allocator);
  }
  else if (allocationSize <= 256) {
   return SubAllocate(bytes, 256, &allocator->free_256, location, allocator);
  }
  else if (allocationSize <= 512) {
   return SubAllocate(bytes, 512, &allocator->free_512, location, allocator);
  }
  else if (allocationSize <= 1024) {
   return SubAllocate(bytes, 1024, &allocator->free_1024, location, allocator);
  }
  else if (allocationSize <= 2048) {
   return SubAllocate(bytes, 2048, &allocator->free_2048, location, allocator);
  }
 }


 // Find enough memory to allocate

 u32 firstPage = FindRange(allocator, numPagesRequested, 0);



 Memory::Assert(firstPage != 0, "Memory::Allocate failed to find enough pages to fufill allocation", 1092, "./mem.cpp");

 SetRange(allocator, firstPage, numPagesRequested);

 if (firstPage == 0 || allocator->size % allocator->pageSize != 0) {
  Memory::Assert(false, "On line: " "1097" ", in file: " "./mem.cpp", 1097, "./mem.cpp");
  return 0; // Fail this allocation in release mode
 }

 // Fill out header
 u8* mem = (u8*)allocator + firstPage * allocator->pageSize;

 u32 alignmentOffset = 0;
 if (alignment != 0) {



  u32 mem_addr = (u32)((void*)mem) + sizeof(Allocation);



  if (mem_addr % alignment != 0) {
   mem_addr = (mem_addr + (alignment - 1)) / alignment * alignment;
   mem = (u8*)(mem_addr - sizeof(Allocation));
  }
 }

 Allocation* allocation = (Allocation*)mem;
 mem += sizeof(Allocation);

 allocation->alignment = alignment;
 allocation->size = bytes;
 allocation->prevOffset = 0;
 allocation->nextOffset = 0;

 allocation->location = location;


 // Track allocated memory
 Memory::Assert(allocation != allocator->active, "On line: " "1131" ", in file: " "./mem.cpp", 1131, "./mem.cpp"); // Should be impossible, but we could have bugs...
 AddtoList(allocator, &allocator->active, allocation);

 // Return memory
 if (allocator->allocateCallback != 0) {
  u8* _mem = (u8*)allocator + firstPage * allocator->pageSize;
  _mem += allocationHeaderPadding;
  Allocation* _allocation = (Allocation*)_mem;
  allocator->allocateCallback(allocator, _allocation, bytes, allocationSize, firstPage, numPagesRequested);
 }

 return mem;
}

void Memory::Allocator::Release(void* memory, const char* location) {
 Memory::Assert(memory != 0, "Memory:Free can't free a null pointer", 1156, "./mem.cpp");
 Allocator* allocator = this;

 // Retrieve allocation information from header. The allocation header always
 // preceeds the allocation.
 u8* mem = (u8*)memory;
 mem -= sizeof(Allocation);
 Allocation* allocation = (Allocation*)mem;
 Memory::Assert(allocation != 0, "Can't free null", 1164, "./mem.cpp");
 u32 alignment = allocation->alignment;

 u32 allocationSize = allocation->size; // Add enough space to pad out for alignment

 u32 allocationHeaderPadding = 0;
 if (alignment != 0) { // Add padding to the header to compensate for alignment
  allocationHeaderPadding = alignment - 1; // Somewhere in this range, we will be aligned
 }
 u32 paddedAllocationSize = allocationSize + allocationHeaderPadding + sizeof(Allocation);
 Memory::Assert(allocationSize != 0, "Memory::Free, double free", 1174, "./mem.cpp");

 Memory::Assert(allocator->requested >= allocation->size, "Memory::Free releasing more memory than was requested", 1176, "./mem.cpp");
 Memory::Assert(allocator->requested != 0, "Memory::Free releasing more memory, but there is nothing to release", 1177, "./mem.cpp");
 allocator->requested -= allocation->size;


 if (alignment == 0) {
  if (paddedAllocationSize <= 64) {
   SubRelease(memory, 64, &allocator->free_64, location, allocator);
   return;
  }
  else if (paddedAllocationSize <= 128) {
   SubRelease(memory, 128, &allocator->free_128, location, allocator);
   return;
  }
  else if (paddedAllocationSize <= 256) {
   SubRelease(memory, 256, &allocator->free_256, location, allocator);
   return;
  }
  else if (paddedAllocationSize <= 512) {
   SubRelease(memory, 512, &allocator->free_512, location, allocator);
   return;
  }
  else if (paddedAllocationSize <= 1024) {
   SubRelease(memory, 1024, &allocator->free_1024, location, allocator);
   return;
  }
  else if (paddedAllocationSize <= 2048) {
   SubRelease(memory, 2048, &allocator->free_2048, location, allocator);
   return;
  }
 }


 // Clear the bits that where tracking this memory
 u8* firstMemory = (u8*)allocator;
 u32 address = (u32)((u8*)mem - (u8*)firstMemory);

 u32 firstPage = address / allocator->pageSize;
 u32 numPages = paddedAllocationSize / allocator->pageSize + (paddedAllocationSize % allocator->pageSize ? 1 : 0);
 ClearRange(allocator, firstPage, numPages);

 // Unlink tracking
 RemoveFromList(allocator, &allocator->active, allocation);

 // Set the size to 0, to indicate that this header has been free-d
 u32 oldSize = allocation->size;
 allocation->size = 0;

 if (allocator->releaseCallback != 0) {
  allocator->releaseCallback(allocator, allocation, oldSize, paddedAllocationSize, firstPage, numPages);
 }
}

namespace Memory {
 namespace Debug {
  class str_const { // constexpr string
  private:
   const char* const p_;
   const ptr_type sz_;
  private:
   str_const& operator= (const str_const& other) = delete;
   str_const(const str_const&& other) = delete;
   str_const& operator= (const str_const&& other) = delete;
  public:
   template<ptr_type N>
   constexpr str_const(const char(&a)[N]) noexcept : // ctor
    p_(a), sz_(N - 1) {
   }
   constexpr char operator[](ptr_type n) const noexcept { // []

    if (n >= sz_) {
     Assert(false, "n >= sz", 1247, "./mem.cpp");
    }
    return p_[n];



   }
   constexpr u32 size() const noexcept { // string length
    return (u32)sz_;
   } // size()
   const char* begin() const noexcept { // start iterator
    return p_;
   } // begin()
   const char* end() const noexcept { // End iterator
    return p_ + sz_;
   } // end()
   template<typename T>
   T& operator<<(T& stream) { // Stream op
    stream << p_;
    return stream;
   } // <<
  };

  u32 u32toa(u8* dest, u32 destSize, u32 num) { // Returns length of string
   Set(dest, 0, destSize, "Memory::Debug::u32toa");

   u32 count = 0;
   u32 tmp = num;
   while (tmp != 0) {
    tmp = tmp / 10;
    count = count + 1;
   }

   if (count == 0) {
    *dest = '0';
    return 1;
   }

   u8* last = dest + count - 1;
   while (num != 0) {
    u32 digit = num % 10;
    num = num / 10;

    *last-- = '0' + digit;
   }

   return count;
  }

  u32 strlen(const u8* str) {
   const u8* s;
   for (s = str; *s; ++s);
   return (u32)(s - str);
  }
 } // namespace Debug
} // namespace Memory

void Memory::Debug::MemInfo(Allocator* allocator, WriteCallback callback, void* userdata) {
 const char* l = "Memory::Debug::DumpAllocationHeaders";

 u8* debugPage = allocator->RequestDbgPage();
 u32 debugSize = allocator->pageSize;

 // Reset memory buffer
 Set(debugPage, 0, debugSize, l);
 u8* i_to_a_buff = debugPage; // Used to convert numbers to strings
 const u32 i_to_a_buff_size = strlen((const u8*)"18446744073709551615") + 1; // u64 max
 u8* mem = i_to_a_buff + i_to_a_buff_size;
 u32 memSize = allocator->pageSize - i_to_a_buff_size;

 { // Tracking %d Pages, %d KiB (%d MiB)
  constexpr str_const out0("Tracking ");
  Copy(mem, out0.begin(), out0.size(), l);
  mem += out0.size();
  memSize -= out0.size();

  u32 numPages = allocator->size / allocator->pageSize;
  Memory::Assert(allocator->size % allocator->pageSize == 0, l, 1324, "./mem.cpp");

  u32 i_len = u32toa(i_to_a_buff, i_to_a_buff_size, numPages);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out1(" pages, Page size: ");
  Copy(mem, out1.begin(), out1.size(), l);
  mem += out1.size();
  memSize -= out1.size();

  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, allocator->pageSize);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out11(" bytes\nTotal memory size: ");
  Copy(mem, out11.begin(), out11.size(), l);
  mem += out11.size();
  memSize -= out11.size();

  u32 kib = allocator->size / 1024;
  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, kib);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out2(" KiB (");
  Copy(mem, out2.begin(), out2.size(), l);
  mem += out2.size();
  memSize -= out2.size();

  u32 mib = kib / 1024;
  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, mib);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out3(" MiB)\n");
  Copy(mem, out3.begin(), out3.size(), l);
  mem += out3.size();
  memSize -= out3.size();
 }

 // Dump what's been written so far
 mem = i_to_a_buff + i_to_a_buff_size;
 callback(mem, (allocator->pageSize - i_to_a_buff_size) - memSize, userdata);

 // Reset memory buffer
 Set(debugPage, 0, debugSize, l);
 i_to_a_buff = debugPage; // Used to convert numbers to strings
 mem = i_to_a_buff + i_to_a_buff_size;
 memSize = allocator->pageSize - i_to_a_buff_size;

 { // Pages: %d free, %d used, %d overhead
  constexpr str_const out0("Page state: ");
  Copy(mem, out0.begin(), out0.size(), l);
  mem += out0.size();
  memSize -= out0.size();

  u32 maskSize = AllocatorPageMaskSize(allocator) / (sizeof(u32) / sizeof(u8)); // convert from u8 to u32
  u32 metaDataSizeBytes = AllocatorPaddedSize() + (maskSize * sizeof(u32));
  u32 numberOfMasksUsed = metaDataSizeBytes / allocator->pageSize;
  if (metaDataSizeBytes % allocator->pageSize != 0) {
   numberOfMasksUsed += 1;
  }
  metaDataSizeBytes = numberOfMasksUsed * allocator->pageSize; // This way, allocatable will start on a page boundary
  // Account for meta data
  metaDataSizeBytes += allocator->pageSize;
  numberOfMasksUsed += 1;

  u32 numPages = allocator->size / allocator->pageSize;
  Memory::Assert(allocator->size % allocator->pageSize == 0, l, 1397, "./mem.cpp");
  u32 usedPages = allocator->numPagesUsed;
  Memory::Assert(usedPages <= numPages, l, 1399, "./mem.cpp");
  u32 freePages = numPages - usedPages;
  u32 overheadPages = metaDataSizeBytes / allocator->pageSize;
  Memory::Assert(usedPages >= overheadPages, l, 1402, "./mem.cpp");
  usedPages -= overheadPages;

  u32 i_len = u32toa(i_to_a_buff, i_to_a_buff_size, freePages);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out1(" free, ");
  Copy(mem, out1.begin(), out1.size(), l);
  mem += out1.size();
  memSize -= out1.size();

  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, usedPages);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out2(" used, ");
  Copy(mem, out2.begin(), out2.size(), l);
  mem += out2.size();
  memSize -= out2.size();

  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, overheadPages);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out3(" overhead\nRequested: ");
  Copy(mem, out3.begin(), out3.size(), l);
  mem += out3.size();
  memSize -= out3.size();

  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, allocator->requested);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out4(" bytes, Served: ");
  Copy(mem, out4.begin(), out4.size(), l);
  mem += out4.size();
  memSize -= out4.size();

  i_len = u32toa(i_to_a_buff, i_to_a_buff_size, usedPages * allocator->pageSize);
  Copy(mem, i_to_a_buff, i_len, l);
  mem += i_len;
  memSize -= i_len;

  constexpr str_const out5(" bytes\n");
  Copy(mem, out5.begin(), out5.size(), l);
  mem += out5.size();
  memSize -= out5.size();
 }

 // Dump what's been written so far
 mem = i_to_a_buff + i_to_a_buff_size;
 callback(mem, (allocator->pageSize - i_to_a_buff_size) - memSize, userdata);

 // Reset memory buffer
 Set(debugPage, 0, debugSize, l);
 i_to_a_buff = debugPage; // Used to convert numbers to strings
 mem = i_to_a_buff + i_to_a_buff_size;
 memSize = allocator->pageSize - i_to_a_buff_size;

 { // Dump active list
  constexpr str_const out0("\nActive allocations:\n");
  Copy(mem, out0.begin(), out0.size(), l);
  mem += out0.size();
  memSize -= out0.size();

  for (Allocation* iter = allocator->active; iter != 0; iter = (iter->nextOffset == 0)? 0 : (Allocation*)((u8*)allocator + iter->nextOffset)) {
   //u64 address = (u64)((void*)iter);
   u64 alloc_address = (u64)((void*)allocator);

   constexpr str_const out5("\t");
   Copy(mem, out5.begin(), out5.size(), l);
   mem += out5.size();
   memSize -= out5.size();

   u32 allocationOffset = (u32)((u8*)iter - (u8*)allocator);
   i32 i_len = u32toa(i_to_a_buff, i_to_a_buff_size, allocationOffset);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   constexpr str_const out2(", size: ");
   Copy(mem, out2.begin(), out2.size(), l);
   mem += out2.size();
   memSize -= out2.size();

   i_len = u32toa(i_to_a_buff, i_to_a_buff_size, iter->size);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   constexpr str_const out3(", padded: ");
   Copy(mem, out3.begin(), out3.size(), l);
   mem += out3.size();
   memSize -= out3.size();

   u32 alignment = iter->alignment;
   u32 allocationHeaderPadding = 0;
   if (alignment != 0) { // Add padding to the header to compensate for alignment
    allocationHeaderPadding = alignment - 1; // Somewhere in this range, we will be aligned
   }

   u32 realSize = iter->size + (u32)(sizeof(Allocation)) + allocationHeaderPadding;
   i_len = u32toa(i_to_a_buff, i_to_a_buff_size, realSize);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   constexpr str_const out6(", alignment: ");
   Copy(mem, out6.begin(), out6.size(), l);
   mem += out6.size();
   memSize -= out6.size();

   i_len = u32toa(i_to_a_buff, i_to_a_buff_size, iter->alignment);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   constexpr str_const outfp(", first page: ");
   Copy(mem, outfp.begin(), outfp.size(), l);
   mem += outfp.size();
   memSize -= outfp.size();

   i_len = u32toa(i_to_a_buff, i_to_a_buff_size, (allocationOffset) / allocator->pageSize);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   constexpr str_const out0(", prev: ");
   Copy(mem, out0.begin(), out0.size(), l);
   mem += out0.size();
   memSize -= out0.size();

   i_len = u32toa(i_to_a_buff, i_to_a_buff_size, iter->prevOffset);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   constexpr str_const out1(", next: ");
   Copy(mem, out1.begin(), out1.size(), l);
   mem += out1.size();
   memSize -= out1.size();

   i_len = u32toa(i_to_a_buff, i_to_a_buff_size, iter->nextOffset);
   Copy(mem, i_to_a_buff, i_len, l);
   mem += i_len;
   memSize -= i_len;

   u32 pathLen = 0;

   if (iter->location != 0) {
    pathLen = strlen((const u8*)iter->location);
   }


   if (memSize < allocator->pageSize / 4 || memSize < (pathLen + pathLen / 4)) { // Drain occasiaonally
    // Dump what's been written so far
    mem = i_to_a_buff + i_to_a_buff_size;
    callback(mem, (allocator->pageSize - i_to_a_buff_size) - memSize, userdata);

    // Reset memory buffer
    Set(debugPage, 0, debugSize, l);
    i_to_a_buff = debugPage; // Used to convert numbers to strings
    mem = i_to_a_buff + i_to_a_buff_size;
    memSize = allocator->pageSize - i_to_a_buff_size;
   }

   constexpr str_const out_loc(", location: ");
   Copy(mem, out_loc.begin(), out_loc.size(), l);
   mem += out_loc.size();
   memSize -= out_loc.size();


   if (iter->location == 0) {



    Memory::Assert(pathLen == 0, "On line: " "1583" ", in file: " "./mem.cpp", 1583, "./mem.cpp");

    constexpr str_const out_loc("null");
    Copy(mem, out_loc.begin(), out_loc.size(), l);
    mem += out_loc.size();
    memSize -= out_loc.size();
   }

   else {
    Memory::Assert(pathLen != 0, "On line: " "1592" ", in file: " "./mem.cpp", 1592, "./mem.cpp");
    Copy(mem, iter->location, pathLen, l);
    mem += pathLen;
    memSize -= pathLen;
   }


   constexpr str_const out4("\n");
   Copy(mem, out4.begin(), out4.size(), l);
   mem += out4.size();
   memSize -= out4.size();
  }

  if (memSize != allocator->pageSize - i_to_a_buff_size) { // Drain if needed
   // Dump what's been written so far
   mem = i_to_a_buff + i_to_a_buff_size;
   callback(mem, (allocator->pageSize - i_to_a_buff_size) - memSize, userdata);

   // Reset memory buffer
   Set(debugPage, 0, debugSize, l);
   i_to_a_buff = debugPage; // Used to convert numbers to strings
   mem = i_to_a_buff + i_to_a_buff_size;
   memSize = allocator->pageSize - i_to_a_buff_size;
  }
 }

 // Reset memory buffer
 Set(debugPage, 0, debugSize, l);
 i_to_a_buff = debugPage; // Used to convert numbers to strings
 mem = i_to_a_buff + i_to_a_buff_size;
 memSize = allocator->pageSize - i_to_a_buff_size;

 constexpr str_const newline("\n\t");
 constexpr str_const isSet("0");
 constexpr str_const notSet("-");

 { // Draw a pretty graph
  u32 numPages = allocator->size / allocator->pageSize;
  u32* mask = (u32*)AllocatorPageMask(allocator);

  constexpr str_const out5("\nPage chart:\n\t");
  Copy(mem, out5.begin(), out5.size(), l);
  mem += out5.size();
  memSize -= out5.size();

  for (u32 i = 0; i < numPages; ++i) {
   u32 m = i / TrackingUnitSize;
   u32 b = i % TrackingUnitSize;

   bool set = mask[m] & (1 << b);
   if (set) {
    Copy(mem, isSet.begin(), isSet.size(), l);
    mem += isSet.size();
    memSize -= isSet.size();
   }
   else {
    Copy(mem, notSet.begin(), notSet.size(), l);
    mem += notSet.size();
    memSize -= notSet.size();
   }

   if ((i + 1) % 80 == 0) {
    Copy(mem, newline.begin(), newline.size(), l);
    mem += newline.size();
    memSize -= newline.size();
   }

   if (memSize < allocator->pageSize / 4) { // Drain occasiaonally
    // Dump what's been written so far
    mem = i_to_a_buff + i_to_a_buff_size;
    callback(mem, (allocator->pageSize - i_to_a_buff_size) - memSize, userdata);

    // Reset memory buffer
    Set(debugPage, 0, debugSize, l);
    i_to_a_buff = debugPage; // Used to convert numbers to strings
    mem = i_to_a_buff + i_to_a_buff_size;
    memSize = allocator->pageSize - i_to_a_buff_size;
   }
  }

  if (memSize != allocator->pageSize - i_to_a_buff_size) { // Drain if needed
   // Dump what's been written so far
   mem = i_to_a_buff + i_to_a_buff_size;
   callback(mem, (allocator->pageSize - i_to_a_buff_size) - memSize, userdata);

   // Reset memory buffer
   Set(debugPage, 0, debugSize, l);
   i_to_a_buff = debugPage; // Used to convert numbers to strings
   mem = i_to_a_buff + i_to_a_buff_size;
   memSize = allocator->pageSize - i_to_a_buff_size;
  }
 }

 allocator->ReleaseDbgPage();
}

void Memory::Debug::PageContent(Allocator* allocator, u32 page, WriteCallback callback, void* userdata) {
 u8* mem = (u8*)allocator + page * allocator->pageSize;
 u32 chunk = allocator->pageSize / 4; // Does not need to be a multiple of 4

 callback(mem, chunk, userdata);
 mem += chunk;
 callback(mem, chunk, userdata);
 mem += chunk;
 callback(mem, chunk, userdata);
 mem += chunk;
 callback(mem, allocator->pageSize - (allocator->pageSize / 4) * 3, userdata);
}

#pragma warning(default:6011)
#pragma warning(default:28182)








float FastSin(float x);
float FastCos(float x);

template<typename T>
struct TVec2 {
 union {
  struct {
   T x;
   T y;
  };
  T v[2];
 };
 inline TVec2() : x(T(0)), y(T(0)) { }
 inline TVec2(T _x, T _y) :
  x(_x), y(_y) { }
 inline TVec2(T* fv) :
  x(fv[0]), y(fv[1]) { }
};

typedef TVec2<float> vec2;
typedef TVec2<int> ivec2;
typedef TVec2<unsigned int> uivec2;

struct vec3 {
 union {
  struct {
   float x;
   float y;
   float z;
  };
  float v[3];
 };
 inline vec3() : x(0.0f), y(0.0f), z(0.0f) { }
 inline vec3(float _x, float _y, float _z) :
  x(_x), y(_y), z(_z) { }
 inline vec3(float* fv) :
  x(fv[0]), y(fv[1]), z(fv[2]) { }
};

vec3 operator+(const vec3& l, const vec3& r);
vec3 operator-(const vec3& l, const vec3& r);
vec3 operator*(const vec3& v, float f);
vec3 operator*(const vec3& l, const vec3& r);
float dot(const vec3& l, const vec3& r);
float lenSq(const vec3& v);
float len(const vec3& v);
void normalize(vec3& v);
vec3 normalized(const vec3& v);
vec3 project(const vec3& a, const vec3& b);
vec3 reject(const vec3& a, const vec3& b);
vec3 reflect(const vec3& a, const vec3& b);
vec3 cross(const vec3& l, const vec3& r);
vec3 lerp(const vec3& s, const vec3& e, float t);
vec3 nlerp(const vec3& s, const vec3& e, float t);
bool operator==(const vec3& l, const vec3& r);
bool operator!=(const vec3& l, const vec3& r);

template<typename T>
struct TVec4 {
 union {
  struct {
   T x;
   T y;
   T z;
   T w;
  };
  T v[4];
 };
 inline TVec4() : x((T)0), y((T)0), z((T)0), w((T)0) { }
 inline TVec4(T _x, T _y, T _z, T _w) :
  x(_x), y(_y), z(_z), w(_w) { }
 inline TVec4(T* fv) :
  x(fv[0]), y(fv[1]), z(fv[2]), w(fv[3]) { }
};

typedef TVec4<float> vec4;
typedef TVec4<int> ivec4;
typedef TVec4<unsigned int> uivec4;


struct mat4 {
 union {
  float v[16];
  struct {
   vec4 right;
   vec4 up;
   vec4 forward;
   vec4 position;
  };
  struct {
   //            row 1     row 2     row 3     row 4
   /* column 1 */float xx; float xy; float xz; float xw;
   /* column 2 */float yx; float yy; float yz; float yw;
   /* column 3 */float zx; float zy; float zz; float zw;
   /* column 4 */float tx; float ty; float tz; float tw;
  };
  struct {
   float c0r0; float c0r1; float c0r2; float c0r3;
   float c1r0; float c1r1; float c1r2; float c1r3;
   float c2r0; float c2r1; float c2r2; float c2r3;
   float c3r0; float c3r1; float c3r2; float c3r3;
  };
  struct {
   float r0c0; float r1c0; float r2c0; float r3c0;
   float r0c1; float r1c1; float r2c1; float r3c1;
   float r0c2; float r1c2; float r2c2; float r3c2;
   float r0c3; float r1c3; float r2c3; float r3c3;
  };
 };
 inline mat4() :
  xx(1), xy(0), xz(0), xw(0),
  yx(0), yy(1), yz(0), yw(0),
  zx(0), zy(0), zz(1), zw(0),
  tx(0), ty(0), tz(0), tw(1) {}

 inline mat4(float* fv) :
  xx(fv[0]), xy(fv[1]), xz(fv[2]), xw(fv[3]),
  yx(fv[4]), yy(fv[5]), yz(fv[6]), yw(fv[7]),
  zx(fv[8]), zy(fv[9]), zz(fv[10]), zw(fv[11]),
  tx(fv[12]), ty(fv[13]), tz(fv[14]), tw(fv[15]) { }

 inline mat4(
  float _00, float _01, float _02, float _03,
  float _10, float _11, float _12, float _13,
  float _20, float _21, float _22, float _23,
  float _30, float _31, float _32, float _33) :
  xx(_00), xy(_01), xz(_02), xw(_03),
  yx(_10), yy(_11), yz(_12), yw(_13),
  zx(_20), zy(_21), zz(_22), zw(_23),
  tx(_30), ty(_31), tz(_32), tw(_33) { }
}; // end mat4 struct

bool operator==(const mat4& a, const mat4& b);
bool operator!=(const mat4& a, const mat4& b);
mat4 operator+(const mat4& a, const mat4& b);
mat4 operator*(const mat4& m, float f);
mat4 operator*(const mat4& a, const mat4& b);
vec4 operator*(const mat4& m, const vec4& v);
vec3 transformVector(const mat4& m, const vec3& v);
vec3 transformPoint(const mat4& m, const vec3& v);
vec3 transformPoint(const mat4& m, const vec3& v, float& w);
void transpose(mat4& m);
mat4 transposed(const mat4& m);
float determinant(const mat4& m);
mat4 adjugate(const mat4& m);
mat4 inverse(const mat4& m);
void invert(mat4& m);
mat4 frustum(float l, float r, float b, float t, float n, float f);
mat4 perspective(float fov, float aspect, float znear, float zfar);
mat4 ortho(float l, float r, float b, float t, float n, float f);
mat4 lookAt(const vec3& position, const vec3& target, const vec3& up);

struct quat {
 union {
  struct {
   float x;
   float y;
   float z;
   float w;
  };
  struct {
   vec3 vector;
   float scalar;
  };
  float v[4];
 };

 inline quat() :
  x(0), y(0), z(0), w(1) { }
 inline quat(float _x, float _y, float _z, float _w) :
  x(_x), y(_y), z(_z), w(_w) {}
};

quat angleAxis(float angle, const vec3& axis);
quat fromTo(const vec3& from, const vec3& to);
vec3 getAxis(const quat& quat);
float getAngle(const quat& quat);
quat operator+(const quat& a, const quat& b);
quat operator-(const quat& a, const quat& b);
quat operator*(const quat& a, float b);
quat operator-(const quat& q);
bool operator==(const quat& left, const quat& right);
bool operator!=(const quat& a, const quat& b);
bool sameOrientation(const quat& left, const quat& right);
float dot(const quat& a, const quat& b);
float lenSq(const quat& q);
float len(const quat& q);
void normalize(quat& q);
quat normalized(const quat& q);
quat conjugate(const quat& q);
quat inverse(const quat& q);
quat operator*(const quat& Q1, const quat& Q2);
vec3 operator*(const quat& q, const vec3& v);
quat mix(const quat& from, const quat& to, float t);
quat nlerp(const quat& from, const quat& to, float t);
quat operator^(const quat& q, float f);
quat operator^(const quat& q, float f);
quat slerp(const quat& start, const quat& end, float t);
quat lookRotation(const vec3& direcion, const vec3& up);
mat4 quatToMat4(const quat& q);
quat mat4ToQuat(const mat4& m);

float Sqrtf(const float& n) {
 if (n == 0.0f) {
  return 0.0f;
 }

 int i = 0x2035AD0C + (*(int*)&n >> 1);
 return n / *(float*)&i + *(float*)&i * 0.25f;
}

float Fabsf(const float& f) {
 if (f < 0.0f) {
  return f * -1.0f;
 }
 return f;
}

double Sin(const double& x) {
 int i = 1;
 double cur = x;
 double acc = 1;
 double fact = 1;
 double pow = x;
 while (acc > .00000001 && i < 100) {
  fact *= ((2 * i) * (2 * i + 1));
  pow *= -1 * x * x;
  acc = pow / fact;
  if (acc < 0.0) { // fabs
   acc *= -1.0;
  }
  cur += acc;
  i++;
 }
 return cur;
}






inline float Hill(float x)
{
 const float a0 = 1.0f;
 const float a2 = 2.0f / (3.1415926535f) - 12.0f / ((3.1415926535f) * (3.1415926535f));
 const float a3 = 16.0f / ((3.1415926535f) * (3.1415926535f) * (3.1415926535f)) - 4.0f / ((3.1415926535f) * (3.1415926535f));
 const float xx = x * x;
 const float xxx = xx * x;

 return a0 + a2 * xx + a3 * xxx;
}

float FastSin(float x)
{
 // wrap x within [0, TWO_PI)
 const float a = x * (1.0f / (2.0f * (3.1415926535f)));
 x -= static_cast<int>(a) * (2.0f * (3.1415926535f));
 if (x < 0.0f)
  x += (2.0f * (3.1415926535f));

 // 4 pieces of hills
 if (x < (0.5f * (3.1415926535f)))
  return Hill((0.5f * (3.1415926535f)) - x);
 else if (x < (3.1415926535f))
  return Hill(x - (0.5f * (3.1415926535f)));
 else if (x < 3.0f * (0.5f * (3.1415926535f)))
  return -Hill(3.0f * (0.5f * (3.1415926535f)) - x);
 else
  return -Hill(x - 3.0f * (0.5f * (3.1415926535f)));
}

float FastCos(float x)
{
 return FastSin(x + (0.5f * (3.1415926535f)));
}

float Tan(const float& d) {
 return FastSin(d) / FastCos(d);
}


vec3 operator+(const vec3& l, const vec3& r) {
 return vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

vec3 operator-(const vec3& l, const vec3& r) {
 return vec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

vec3 operator*(const vec3& v, float f) {
 return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator*(const vec3& l, const vec3& r) {
 return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

float dot(const vec3& l, const vec3& r) {
 return l.x * r.x + l.y * r.y + l.z * r.z;
}

float lenSq(const vec3& v) {
 return v.x * v.x + v.y * v.y + v.z * v.z;
}

float len(const vec3& v) {
 float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
 if (lenSq < 0.000001f) {
  return 0.0f;
 }
 return Sqrtf(lenSq);
}

void normalize(vec3& v) {
 float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
 if (lenSq < 0.000001f) {
  return;
 }
 float invLen = 1.0f / Sqrtf(lenSq);

 v.x *= invLen;
 v.y *= invLen;
 v.z *= invLen;
}

vec3 normalized(const vec3& v) {
 float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
 if (lenSq < 0.000001f) {
  return v;
 }
 float invLen = 1.0f / Sqrtf(lenSq);

 return vec3(
  v.x * invLen,
  v.y * invLen,
  v.z * invLen
 );
}

vec3 project(const vec3& a, const vec3& b) {
 float magBSq = len(b);
 if (magBSq < 0.000001f) {
  return vec3();
 }
 float scale = dot(a, b) / magBSq;
 return b * scale;
}

vec3 reject(const vec3& a, const vec3& b) {
 vec3 projection = project(a, b);
 return a - projection;
}

vec3 reflect(const vec3& a, const vec3& b) {
 float magBSq = len(b);
 if (magBSq < 0.000001f) {
  return vec3();
 }
 float scale = dot(a, b) / magBSq;
 vec3 proj2 = b * (scale * 2);
 return a - proj2;
}

vec3 cross(const vec3& l, const vec3& r) {
 return vec3(
  l.y * r.z - l.z * r.y,
  l.z * r.x - l.x * r.z,
  l.x * r.y - l.y * r.x
 );
}

vec3 lerp(const vec3& s, const vec3& e, float t) {
 return vec3(
  s.x + (e.x - s.x) * t,
  s.y + (e.y - s.y) * t,
  s.z + (e.z - s.z) * t
 );
}

vec3 nlerp(const vec3& s, const vec3& e, float t) {
 vec3 linear(
  s.x + (e.x - s.x) * t,
  s.y + (e.y - s.y) * t,
  s.z + (e.z - s.z) * t
 );
 return normalized(linear);
}

bool operator==(const vec3& l, const vec3& r) {
 vec3 diff(l - r);
 return lenSq(diff) < 0.000001f;
}

bool operator!=(const vec3& l, const vec3& r) {
 return !(l == r);
}

bool operator==(const mat4& a, const mat4& b) {
 for (int i = 0; i < 16; ++i) {
  if (Fabsf(a.v[i] - b.v[i]) > 0.000001f) {
   return false;
  }
 }
 return true;
}

bool operator!=(const mat4& a, const mat4& b) {
 return !(a == b);
}

mat4 operator+(const mat4& a, const mat4& b) {
 return mat4(
  a.xx + b.xx, a.xy + b.xy, a.xz + b.xz, a.xw + b.xw,
  a.yx + b.yx, a.yy + b.yy, a.yz + b.yz, a.yw + b.yw,
  a.zx + b.zx, a.zy + b.zy, a.zz + b.zz, a.zw + b.zw,
  a.tx + b.tx, a.ty + b.ty, a.tz + b.tz, a.tw + b.tw
 );
}

mat4 operator*(const mat4& m, float f) {
 return mat4(
  m.xx * f, m.xy * f, m.xz * f, m.xw * f,
  m.yx * f, m.yy * f, m.yz * f, m.yw * f,
  m.zx * f, m.zy * f, m.zz * f, m.zw * f,
  m.tx * f, m.ty * f, m.tz * f, m.tw * f
 );
}







mat4 operator*(const mat4& a, const mat4& b) {
 return mat4(
  a.v[0 * 4 + 0] * b.v[0 * 4 + 0] + a.v[1 * 4 + 0] * b.v[0 * 4 + 1] + a.v[2 * 4 + 0] * b.v[0 * 4 + 2] + a.v[3 * 4 + 0] * b.v[0 * 4 + 3], a.v[0 * 4 + 1] * b.v[0 * 4 + 0] + a.v[1 * 4 + 1] * b.v[0 * 4 + 1] + a.v[2 * 4 + 1] * b.v[0 * 4 + 2] + a.v[3 * 4 + 1] * b.v[0 * 4 + 3], a.v[0 * 4 + 2] * b.v[0 * 4 + 0] + a.v[1 * 4 + 2] * b.v[0 * 4 + 1] + a.v[2 * 4 + 2] * b.v[0 * 4 + 2] + a.v[3 * 4 + 2] * b.v[0 * 4 + 3], a.v[0 * 4 + 3] * b.v[0 * 4 + 0] + a.v[1 * 4 + 3] * b.v[0 * 4 + 1] + a.v[2 * 4 + 3] * b.v[0 * 4 + 2] + a.v[3 * 4 + 3] * b.v[0 * 4 + 3], // Column 0
  a.v[0 * 4 + 0] * b.v[1 * 4 + 0] + a.v[1 * 4 + 0] * b.v[1 * 4 + 1] + a.v[2 * 4 + 0] * b.v[1 * 4 + 2] + a.v[3 * 4 + 0] * b.v[1 * 4 + 3], a.v[0 * 4 + 1] * b.v[1 * 4 + 0] + a.v[1 * 4 + 1] * b.v[1 * 4 + 1] + a.v[2 * 4 + 1] * b.v[1 * 4 + 2] + a.v[3 * 4 + 1] * b.v[1 * 4 + 3], a.v[0 * 4 + 2] * b.v[1 * 4 + 0] + a.v[1 * 4 + 2] * b.v[1 * 4 + 1] + a.v[2 * 4 + 2] * b.v[1 * 4 + 2] + a.v[3 * 4 + 2] * b.v[1 * 4 + 3], a.v[0 * 4 + 3] * b.v[1 * 4 + 0] + a.v[1 * 4 + 3] * b.v[1 * 4 + 1] + a.v[2 * 4 + 3] * b.v[1 * 4 + 2] + a.v[3 * 4 + 3] * b.v[1 * 4 + 3], // Column 1
  a.v[0 * 4 + 0] * b.v[2 * 4 + 0] + a.v[1 * 4 + 0] * b.v[2 * 4 + 1] + a.v[2 * 4 + 0] * b.v[2 * 4 + 2] + a.v[3 * 4 + 0] * b.v[2 * 4 + 3], a.v[0 * 4 + 1] * b.v[2 * 4 + 0] + a.v[1 * 4 + 1] * b.v[2 * 4 + 1] + a.v[2 * 4 + 1] * b.v[2 * 4 + 2] + a.v[3 * 4 + 1] * b.v[2 * 4 + 3], a.v[0 * 4 + 2] * b.v[2 * 4 + 0] + a.v[1 * 4 + 2] * b.v[2 * 4 + 1] + a.v[2 * 4 + 2] * b.v[2 * 4 + 2] + a.v[3 * 4 + 2] * b.v[2 * 4 + 3], a.v[0 * 4 + 3] * b.v[2 * 4 + 0] + a.v[1 * 4 + 3] * b.v[2 * 4 + 1] + a.v[2 * 4 + 3] * b.v[2 * 4 + 2] + a.v[3 * 4 + 3] * b.v[2 * 4 + 3], // Column 2
  a.v[0 * 4 + 0] * b.v[3 * 4 + 0] + a.v[1 * 4 + 0] * b.v[3 * 4 + 1] + a.v[2 * 4 + 0] * b.v[3 * 4 + 2] + a.v[3 * 4 + 0] * b.v[3 * 4 + 3], a.v[0 * 4 + 1] * b.v[3 * 4 + 0] + a.v[1 * 4 + 1] * b.v[3 * 4 + 1] + a.v[2 * 4 + 1] * b.v[3 * 4 + 2] + a.v[3 * 4 + 1] * b.v[3 * 4 + 3], a.v[0 * 4 + 2] * b.v[3 * 4 + 0] + a.v[1 * 4 + 2] * b.v[3 * 4 + 1] + a.v[2 * 4 + 2] * b.v[3 * 4 + 2] + a.v[3 * 4 + 2] * b.v[3 * 4 + 3], a.v[0 * 4 + 3] * b.v[3 * 4 + 0] + a.v[1 * 4 + 3] * b.v[3 * 4 + 1] + a.v[2 * 4 + 3] * b.v[3 * 4 + 2] + a.v[3 * 4 + 3] * b.v[3 * 4 + 3] // Column 3
 );
}







vec4 operator*(const mat4& m, const vec4& v) {
 return vec4(
  v.x * m.v[0 * 4 + 0] + v.y * m.v[1 * 4 + 0] + v.z * m.v[2 * 4 + 0] + v.w * m.v[3 * 4 + 0],
  v.x * m.v[0 * 4 + 1] + v.y * m.v[1 * 4 + 1] + v.z * m.v[2 * 4 + 1] + v.w * m.v[3 * 4 + 1],
  v.x * m.v[0 * 4 + 2] + v.y * m.v[1 * 4 + 2] + v.z * m.v[2 * 4 + 2] + v.w * m.v[3 * 4 + 2],
  v.x * m.v[0 * 4 + 3] + v.y * m.v[1 * 4 + 3] + v.z * m.v[2 * 4 + 3] + v.w * m.v[3 * 4 + 3]
 );
}

vec3 transformVector(const mat4& m, const vec3& v) {
 return vec3(
  v.x * m.v[0 * 4 + 0] + v.y * m.v[1 * 4 + 0] + v.z * m.v[2 * 4 + 0] + 0.0f * m.v[3 * 4 + 0],
  v.x * m.v[0 * 4 + 1] + v.y * m.v[1 * 4 + 1] + v.z * m.v[2 * 4 + 1] + 0.0f * m.v[3 * 4 + 1],
  v.x * m.v[0 * 4 + 2] + v.y * m.v[1 * 4 + 2] + v.z * m.v[2 * 4 + 2] + 0.0f * m.v[3 * 4 + 2]
 );
}

vec3 transformPoint(const mat4& m, const vec3& v) {
 return vec3(
  v.x * m.v[0 * 4 + 0] + v.y * m.v[1 * 4 + 0] + v.z * m.v[2 * 4 + 0] + 1.0f * m.v[3 * 4 + 0],
  v.x * m.v[0 * 4 + 1] + v.y * m.v[1 * 4 + 1] + v.z * m.v[2 * 4 + 1] + 1.0f * m.v[3 * 4 + 1],
  v.x * m.v[0 * 4 + 2] + v.y * m.v[1 * 4 + 2] + v.z * m.v[2 * 4 + 2] + 1.0f * m.v[3 * 4 + 2]
 );
}

vec3 transformPoint(const mat4& m, const vec3& v, float& w) {
 float _w = w;
 w = v.x * m.v[0 * 4 + 3] + v.y * m.v[1 * 4 + 3] + v.z * m.v[2 * 4 + 3] + _w * m.v[3 * 4 + 3];

 return vec3(
  v.x * m.v[0 * 4 + 0] + v.y * m.v[1 * 4 + 0] + v.z * m.v[2 * 4 + 0] + _w * m.v[3 * 4 + 0],
  v.x * m.v[0 * 4 + 1] + v.y * m.v[1 * 4 + 1] + v.z * m.v[2 * 4 + 1] + _w * m.v[3 * 4 + 1],
  v.x * m.v[0 * 4 + 2] + v.y * m.v[1 * 4 + 2] + v.z * m.v[2 * 4 + 2] + _w * m.v[3 * 4 + 2]
 );
}




void transpose(mat4& m) {
 {float t = m.yx; m.yx = m.xy; m.xy = t; };
 {float t = m.zx; m.zx = m.xz; m.xz = t; };
 {float t = m.tx; m.tx = m.xw; m.xw = t; };
 {float t = m.zy; m.zy = m.yz; m.yz = t; };
 {float t = m.ty; m.ty = m.yw; m.yw = t; };
 {float t = m.tz; m.tz = m.zw; m.zw = t; };
}

mat4 transposed(const mat4& m) {
 return mat4(
  m.xx, m.yx, m.zx, m.tx,
  m.xy, m.yy, m.zy, m.ty,
  m.xz, m.yz, m.zz, m.tz,
  m.xw, m.yw, m.zw, m.tw
 );
}






float determinant(const mat4& m) {
 return m.v[0] * (m.v[1 * 4 + 1] * (m.v[2 * 4 + 2] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[2 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 2]))
  - m.v[4] * (m.v[0 * 4 + 1] * (m.v[2 * 4 + 2] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[2 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 2]))
  + m.v[8] * (m.v[0 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[1 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 2]))
  - m.v[12] * (m.v[0 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 2]) - m.v[1 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 2]) + m.v[2 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 2]));
}

mat4 adjugate(const mat4& m) {
 // Cofactor(M[i, j]) = Minor(M[i, j]] * pow(-1, i + j)
 mat4 cofactor;

 cofactor.v[0] = (m.v[1 * 4 + 1] * (m.v[2 * 4 + 2] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[2 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 2]));
 cofactor.v[1] = -(m.v[1 * 4 + 0] * (m.v[2 * 4 + 2] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[2 * 4 + 0] * (m.v[1 * 4 + 2] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 0] * (m.v[1 * 4 + 2] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 2]));
 cofactor.v[2] = (m.v[1 * 4 + 0] * (m.v[2 * 4 + 1] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 1]) - m.v[2 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 1]) + m.v[3 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 1]));
 cofactor.v[3] = -(m.v[1 * 4 + 0] * (m.v[2 * 4 + 1] * m.v[3 * 4 + 2] - m.v[2 * 4 + 2] * m.v[3 * 4 + 1]) - m.v[2 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[3 * 4 + 2] - m.v[1 * 4 + 2] * m.v[3 * 4 + 1]) + m.v[3 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[2 * 4 + 2] - m.v[1 * 4 + 2] * m.v[2 * 4 + 1]));

 cofactor.v[4] = -(m.v[0 * 4 + 1] * (m.v[2 * 4 + 2] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[2 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 2]));
 cofactor.v[5] = (m.v[0 * 4 + 0] * (m.v[2 * 4 + 2] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[2 * 4 + 0] * (m.v[0 * 4 + 2] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 0] * (m.v[0 * 4 + 2] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 2]));
 cofactor.v[6] = -(m.v[0 * 4 + 0] * (m.v[2 * 4 + 1] * m.v[3 * 4 + 3] - m.v[2 * 4 + 3] * m.v[3 * 4 + 1]) - m.v[2 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 1]) + m.v[3 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 1]));
 cofactor.v[7] = (m.v[0 * 4 + 0] * (m.v[2 * 4 + 1] * m.v[3 * 4 + 2] - m.v[2 * 4 + 2] * m.v[3 * 4 + 1]) - m.v[2 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[3 * 4 + 2] - m.v[0 * 4 + 2] * m.v[3 * 4 + 1]) + m.v[3 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[2 * 4 + 2] - m.v[0 * 4 + 2] * m.v[2 * 4 + 1]));

 cofactor.v[8] = (m.v[0 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[1 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 2]));
 cofactor.v[9] = -(m.v[0 * 4 + 0] * (m.v[1 * 4 + 2] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 2]) - m.v[1 * 4 + 0] * (m.v[0 * 4 + 2] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 2]) + m.v[3 * 4 + 0] * (m.v[0 * 4 + 2] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 2]));
 cofactor.v[10] = (m.v[0 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[3 * 4 + 3] - m.v[1 * 4 + 3] * m.v[3 * 4 + 1]) - m.v[1 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[3 * 4 + 3] - m.v[0 * 4 + 3] * m.v[3 * 4 + 1]) + m.v[3 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 1]));
 cofactor.v[11] = -(m.v[0 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[3 * 4 + 2] - m.v[1 * 4 + 2] * m.v[3 * 4 + 1]) - m.v[1 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[3 * 4 + 2] - m.v[0 * 4 + 2] * m.v[3 * 4 + 1]) + m.v[3 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[1 * 4 + 2] - m.v[0 * 4 + 2] * m.v[1 * 4 + 1]));

 cofactor.v[12] = -(m.v[0 * 4 + 1] * (m.v[1 * 4 + 2] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 2]) - m.v[1 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 2]) + m.v[2 * 4 + 1] * (m.v[0 * 4 + 2] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 2]));
 cofactor.v[13] = (m.v[0 * 4 + 0] * (m.v[1 * 4 + 2] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 2]) - m.v[1 * 4 + 0] * (m.v[0 * 4 + 2] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 2]) + m.v[2 * 4 + 0] * (m.v[0 * 4 + 2] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 2]));
 cofactor.v[14] = -(m.v[0 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[2 * 4 + 3] - m.v[1 * 4 + 3] * m.v[2 * 4 + 1]) - m.v[1 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[2 * 4 + 3] - m.v[0 * 4 + 3] * m.v[2 * 4 + 1]) + m.v[2 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[1 * 4 + 3] - m.v[0 * 4 + 3] * m.v[1 * 4 + 1]));
 cofactor.v[15] = (m.v[0 * 4 + 0] * (m.v[1 * 4 + 1] * m.v[2 * 4 + 2] - m.v[1 * 4 + 2] * m.v[2 * 4 + 1]) - m.v[1 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[2 * 4 + 2] - m.v[0 * 4 + 2] * m.v[2 * 4 + 1]) + m.v[2 * 4 + 0] * (m.v[0 * 4 + 1] * m.v[1 * 4 + 2] - m.v[0 * 4 + 2] * m.v[1 * 4 + 1]));

 return transposed(cofactor);
}

mat4 inverse(const mat4& m) {
 float det = determinant(m);

 if (det == 0.0f) { // Epsilon check would need to be REALLY small
  return mat4();
 }
 mat4 adj = adjugate(m);

 return adj * (1.0f / det);
}

void invert(mat4& m) {
 float det = determinant(m);

 if (det == 0.0f) {
  m = mat4();
  return;
 }

 m = adjugate(m) * (1.0f / det);
}

mat4 frustum(float l, float r, float b, float t, float n, float f) {
 if (l == r || t == b || n == f) {
  return mat4(); // Error
 }
 return mat4(
  (2.0f * n) / (r - l), 0, 0, 0,
  0, (2.0f * n) / (t - b), 0, 0,
  (r + l) / (r - l), (t + b) / (t - b), (-(f + n)) / (f - n), -1,
  0, 0, (-2 * f * n) / (f - n), 0
 );
}

mat4 perspective(float fov, float aspect, float znear, float zfar) {
 float ymax = znear * (float)Tan(fov * 3.14159265359f / 360.0f);
 float xmax = ymax * aspect;

 return frustum(-xmax, xmax, -ymax, ymax, znear, zfar);
}

mat4 ortho(float l, float r, float b, float t, float n, float f) {
 if (l == r || t == b || n == f) {
  return mat4(); // Error
 }
 return mat4(
  2.0f / (r - l), 0, 0, 0,
  0, 2.0f / (t - b), 0, 0,
  0, 0, -2.0f / (f - n), 0,
  -((r + l) / (r - l)), -((t + b) / (t - b)), -((f + n) / (f - n)), 1
 );
}

mat4 lookAt(const vec3& position, const vec3& target, const vec3& up) {
 // Remember, forward is negative z
 vec3 f = normalized(target - position) * -1.0f;
 vec3 r = cross(up, f); // Right handed
 if (r == vec3(0, 0, 0)) {
  return mat4(); // Error
 }
 normalize(r);
 vec3 u = normalized(cross(f, r)); // Right handed

 vec3 t = vec3(
  -dot(r, position),
  -dot(u, position),
  -dot(f, position)
 );

 return mat4(
  // Transpose upper 3x3 matrix to invert it
  r.x, u.x, f.x, 0,
  r.y, u.y, f.y, 0,
  r.z, u.z, f.z, 0,
  t.x, t.y, t.z, 1
 );
}

quat angleAxis(float angle, const vec3& axis) {
 vec3 norm = normalized(axis);
 float s = FastSin(angle * 0.5f);

 return quat(
  norm.x * s,
  norm.y * s,
  norm.z * s,
  FastCos(angle * 0.5f)
 );
}

quat fromTo(const vec3& from, const vec3& to) {
 vec3 f = normalized(from);
 vec3 t = normalized(to);

 if (f == t) {
  return quat();
 }
 else if (f == t * -1.0f) {
  vec3 ortho = vec3(1, 0, 0);
  if (Fabsf(f.y) < Fabsf(f.x)) {
   ortho = vec3(0, 1, 0);
  }
  if (Fabsf(f.z) < Fabsf(f.y) && Fabsf(f.z) < Fabsf(f.x)) {
   ortho = vec3(0, 0, 1);
  }

  vec3 axis = normalized(cross(f, ortho));
  return quat(axis.x, axis.y, axis.z, 0);
 }

 vec3 half = normalized(f + t);
 vec3 axis = cross(f, half);

 return quat(
  axis.x,
  axis.y,
  axis.z,
  dot(f, half)
 );
}

vec3 getAxis(const quat& quat) {
 return normalized(vec3(quat.x, quat.y, quat.z));
}

float getAngle(const quat& quat) {
 return 2.0f * (1.0f / FastCos(quat.w));
}

quat operator+(const quat& a, const quat& b) {
 return quat(
  a.x + b.x,
  a.y + b.y,
  a.z + b.z,
  a.w + b.w
 );
}

quat operator-(const quat& a, const quat& b) {
 return quat(
  a.x - b.x,
  a.y - b.y,
  a.z - b.z,
  a.w - b.w
 );
}

quat operator*(const quat& a, float b) {
 return quat(
  a.x * b,
  a.y * b,
  a.z * b,
  a.w * b
 );
}

quat operator-(const quat& q) {
 return quat(
  -q.x,
  -q.y,
  -q.z,
  -q.w
 );
}

bool operator==(const quat& left, const quat& right) {
 return (Fabsf(left.x - right.x) <= 0.000001f &&
  Fabsf(left.y - right.y) <= 0.000001f &&
  Fabsf(left.z - right.z) <= 0.000001f &&
  Fabsf(left.w - left.w) <= 0.000001f);
}

bool operator!=(const quat& a, const quat& b) {
 return !(a == b);
}

bool sameOrientation(const quat& left, const quat& right) {
 return (Fabsf(left.x - right.x) <= 0.000001f && Fabsf(left.y - right.y) <= 0.000001f &&
  Fabsf(left.z - right.z) <= 0.000001f && Fabsf(left.w - left.w) <= 0.000001f)
  || (Fabsf(left.x + right.x) <= 0.000001f && Fabsf(left.y + right.y) <= 0.000001f &&
   Fabsf(left.z + right.z) <= 0.000001f && Fabsf(left.w + left.w) <= 0.000001f);
}

float dot(const quat& a, const quat& b) {
 return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float lenSq(const quat& q) {
 return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float len(const quat& q) {
 float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
 if (lenSq < 0.000001f) {
  return 0.0f;
 }
 return Sqrtf(lenSq);
}

void normalize(quat& q) {
 float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
 if (lenSq < 0.000001f) {
  return;
 }
 float i_len = 1.0f / Sqrtf(lenSq);

 q.x *= i_len;
 q.y *= i_len;
 q.z *= i_len;
 q.w *= i_len;
}

quat normalized(const quat& q) {
 float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
 if (lenSq < 0.000001f) {
  return quat();
 }
 float i_len = 1.0f / Sqrtf(lenSq);

 return quat(
  q.x * i_len,
  q.y * i_len,
  q.z * i_len,
  q.w * i_len
 );
}

quat conjugate(const quat& q) {
 return quat(
  -q.x,
  -q.y,
  -q.z,
  q.w
 );
}

quat inverse(const quat& q) {
 float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
 if (lenSq < 0.000001f) {
  return quat();
 }
 float recip = 1.0f / lenSq;

 // conjugate / norm
 return quat(
  -q.x * recip,
  -q.y * recip,
  -q.z * recip,
  q.w * recip
 );
}


quat operator*(const quat& Q1, const quat& Q2) {
 return quat(
  Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
  -Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
  Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
  -Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
 );
}
vec3 operator*(const quat& q, const vec3& v) {
 return q.vector * 2.0f * dot(q.vector, v) +
  v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
  cross(q.vector, v) * 2.0f * q.scalar;
}

quat mix(const quat& from, const quat& to, float t) {
 return from * (1.0f - t) + to * t;
}

quat nlerp(const quat& from, const quat& to, float t) {
 return normalized(from + (to - from) * t);
}

quat operator^(const quat& q, float f) {
 float angle = 2.0f * (1.0f / FastCos(q.scalar));
 vec3 axis = normalized(q.vector);

 float halfCos = FastCos(f * angle * 0.5f);
 float halfSin = FastSin(f * angle * 0.5f);

 return quat(
  axis.x * halfSin,
  axis.y * halfSin,
  axis.z * halfSin,
  halfCos
 );
}

quat slerp(const quat& start, const quat& end, float t) {
 if (Fabsf(dot(start, end)) > 1.0f - 0.000001f) {
  return nlerp(start, end, t);
 }

 return normalized(((inverse(start) * end) ^ t) * start);
}

quat lookRotation(const vec3& direcion, const vec3& up) {
 // Find orthonormal basis vectors
 vec3 f = normalized(direcion);
 vec3 u = normalized(up);
 vec3 r = cross(u, f);
 u = cross(f, r);

 // From world forward to object forward
 quat f2d = fromTo(vec3(0, 0, 1), f);

 // what direction is the new object up?
 vec3 objectUp = f2d * vec3(0, 1, 0);
 // From object up to desired up
 quat u2u = fromTo(objectUp, u);

 // Rotate to forward direction first, then twist to correct up
 quat result = f2d * u2u;
 // Don�t forget to normalize the result
 return normalized(result);
}

mat4 quatToMat4(const quat& q) {
 vec3 r = q * vec3(1, 0, 0);
 vec3 u = q * vec3(0, 1, 0);
 vec3 f = q * vec3(0, 0, 1);

 return mat4(
  r.x, r.y, r.z, 0,
  u.x, u.y, u.z, 0,
  f.x, f.y, f.z, 0,
  0, 0, 0, 1
 );
}

quat mat4ToQuat(const mat4& m) {
 vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z));
 vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z));
 vec3 right = cross(up, forward);
 up = cross(forward, right);

 return lookRotation(forward, up);
}

Memory::Allocator* wasmGraphics_GlobalAllocator = 0;

__attribute__ (( visibility( "default" ) )) extern "C" void wasmGraphics_SetGlobalAllocator(Memory::Allocator* alloc) {
    wasmGraphics_GlobalAllocator = alloc;
}

__attribute__ (( visibility( "default" ) )) extern "C" void* wasmGraphics_AllocateMem(u32 bytes) {
 return wasmGraphics_GlobalAllocator->Allocate(bytes, 0, "JS bridge");
}

__attribute__ (( visibility( "default" ) )) extern "C" void wasmGraphics_ReleaseMem(void* mem) {
    wasmGraphics_GlobalAllocator->Release(mem, "JS bridge");
}

typedef void* (*fpWasmGraphicsAllocateMem)(u32 bytes);
typedef void (*fpWasmGraphicsReleaseMem)(void* mem);

__attribute__ (( visibility( "default" ) )) extern "C" fpWasmGraphicsAllocateMem wasmGraphics_GetAlloceFunction() {
    return wasmGraphics_AllocateMem;
}

__attribute__ (( visibility( "default" ) )) extern "C" fpWasmGraphicsReleaseMem wasmGraphics_GetReleaseFunction() {
    return wasmGraphics_ReleaseMem;
}




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


namespace Internal {
 void CalculateTangentArray(unsigned int vertexCount, vec3* vertex, vec3* normal,
  vec2* texcoord, vec3* outTangent)
 {
  vec3* tan1 = (vec3*)wasmGraphics_AllocateMem(vertexCount * 2 * sizeof(vec3));
  vec3* tan2 = tan1 + vertexCount;

  Memory::Set(tan1, 0, vertexCount * 2 * sizeof(vec3), "On line: " "11" ", in file: " "./FileLoadersWASM.cpp");

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

  wasmGraphics_ReleaseMem(tan1);
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

__attribute__ (( visibility( "default" ) )) extern "C" void FinishLoadingText(const char* path, OnTextFileLoaded triggerThisCallback, void* withThisData, u32 whichHasThisManyBytes) {
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

__attribute__ (( visibility( "default" ) )) extern "C" void FinishLoadingMesh(const char* path, OnMeshLoaded triggerThisCallback, void* withThisData, u32 whichHasThisManyBytes) {
 unsigned int* uint_data = (unsigned int*)withThisData;
 unsigned int sizes[3];
 sizes[0] = uint_data[0];
 sizes[1] = uint_data[1];
 sizes[2] = uint_data[2];

 unsigned int mem_needed = sizeof(MeshFile)
  + sizeof(float) * 3 * sizes[0]; // tan

 void* mem = wasmGraphics_AllocateMem(mem_needed + 1);
 unsigned char* iter = (unsigned char* )mem;

 MeshFile* result = (MeshFile*)iter;
 iter += sizeof(MeshFile);

 float* pos = (float*)(uint_data + 3);
 float* nrm = pos + sizes[0] * 3;
 float* tex = nrm + sizes[1] * 3;

 result->tan = (float*)iter;
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
 wasmGraphics_ReleaseMem(file);
}

extern "C" void wasmFileLoaderLoadTexture(const char* path, int len, OnTextureLoaded callback);

__attribute__ (( visibility( "default" ) )) extern "C" void FinishLoadingTexture(const char* path, OnTextureLoaded triggerThisCallback, void* withThisData, u32 whichHasThisManyBytes) {
 unsigned int* uint_data = (unsigned int*)withThisData;
 unsigned int sizes[3];
 sizes[0] = uint_data[0];
 sizes[1] = uint_data[1];
 sizes[2] = uint_data[2];

 unsigned int mem_needed = sizeof(TextureFile);
 TextureFile* result = (TextureFile*)wasmGraphics_AllocateMem(mem_needed);

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
 Graphics::Internal::Assert(file->data != 0, "On line: " "176" ", in file: " "./FileLoadersWASM.cpp", "Can't free null pointer");
 unsigned int* uint_data = ((unsigned int*)file->data) - 3;
 Graphics::Internal::Assert(uint_data[0] == file->width, "On line: " "178" ", in file: " "./FileLoadersWASM.cpp", "Bad width");
 Graphics::Internal::Assert(uint_data[1] == file->height, "On line: " "179" ", in file: " "./FileLoadersWASM.cpp", "Bad height");
 Graphics::Internal::Assert(uint_data[2] == file->channels, "On line: " "180" ", in file: " "./FileLoadersWASM.cpp", "Bad channel count");
 wasmGraphics_ReleaseMem(uint_data);
 wasmGraphics_ReleaseMem(file);
}






Graphics::Index gLightmapMVP;

Graphics::FrameBuffer* gLightmapFBO;
Graphics::Texture* gLightmapDepth;
Graphics::Texture* gLightmapColor;

Graphics::Shader* gLightmapDrawShader;
Graphics::Shader* gLightmapBlitShader;
Graphics::Index gLightmapFboAttachment;
Graphics::VertexLayout* gLightmapMesh;
Graphics::VertexLayout* gLightmapSkullLayout;
Graphics::VertexLayout* gLightmapPlaneLayout;

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


Graphics::Buffer* gBuffers[10];

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
 gLightmapColor = gfx->CreateTexture(Graphics::TextureFormat::RGBA8, 512, 512);
 gLightmapDepth = gfx->CreateTexture(Graphics::TextureFormat::Depth, 512, 512, 0, Graphics::TextureFormat::Depth, false);
 //gLightmapFBO->AttachColor(*gLightmapColor);
 gLightmapFBO->AttachDepth(*gLightmapDepth);
 Graphics::Internal::Assert(gLightmapFBO->IsValid(), "On line: " "121" ", in file: " "./sample.cpp", "Invalid fbo");

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
  Graphics::Internal::Assert(lightmap_vShader->length != 0, "On line: " "136" ", in file: " "./sample.cpp", "Empty lightmap.vert");
  Graphics::Internal::Assert(lightmap_vShader->text != 0, "On line: " "137" ", in file: " "./sample.cpp", "No text pointer in lightmap.vert");
  Graphics::Internal::Assert(*lightmap_vShader->text != 0, "On line: " "138" ", in file: " "./sample.cpp", "Empty string in lightmap.vert");
  //GraphicsAssert(*lightmap_vShader->text == 0, file->text );
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
 Graphics::Internal::Assert(lightmap_vShader->length != 0, "On line: " "204" ", in file: " "./sample.cpp", "2c Empty lightmap.vert");
 Graphics::Internal::Assert(lightmap_vShader->text != 0, "On line: " "205" ", in file: " "./sample.cpp", "2 No text pointer in lightmap.vert");
 Graphics::Internal::Assert(*lightmap_vShader->text != 0, "On line: " "206" ", in file: " "./sample.cpp", "2 Empty string in lightmap.vert");
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
  -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
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

 gLightmapMVP = gLightmapDrawShader->GetUniform("mvp");
 Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "411" ", in file: " "./sample.cpp", "INvalid lightmap mvp?");

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
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "428" ", in file: " "./sample.cpp", "(3) INvalid lightmap mvp?");
  return;
 }
 Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "431" ", in file: " "./sample.cpp", "(4) INvalid lightmap mvp?");

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



}

void Render(Graphics::Device * gfx, int x, int y, int w, int h) {
 if (!IsRunning) {
  return;
 }
 if (!isFinishedInitializing) {
  return;
 }
 IsRunning = false;


 Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "474" ", in file: " "./sample.cpp", "(5) INvalid lightmap mvp?");

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
  gfx->SetViewport(0, 0, 512, 512);
  gfx->Clear(1.0f, 1.0f, 1.0f, 1.0f);

  gfx->Bind(gLightmapDrawShader);
  mat4 mvp = ShadowProjection * ShadowView * model1;
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "518" ", in file: " "./sample.cpp", "(2) INvalid lightmap mvp?");
  gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "520" ", in file: " "./sample.cpp", "(a) INvalid lightmap mvp?");
  gfx->Draw(*gLightmapSkullLayout, Graphics::DrawMode::Triangles, 0, gLightmapSkullLayout->GetUserData());

  mvp = ShadowProjection* ShadowView* model2;
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "524" ", in file: " "./sample.cpp", "(b) INvalid lightmap mvp?");
  gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "526" ", in file: " "./sample.cpp", "(c) INvalid lightmap mvp?");
  gfx->Draw(*gLightmapSkullLayout, Graphics::DrawMode::Triangles, 0, gLightmapSkullLayout->GetUserData());

  mvp = ShadowProjection * ShadowView * model3;
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "530" ", in file: " "./sample.cpp", "(d) INvalid lightmap mvp?");
  gfx->Bind(gLightmapMVP, Graphics::UniformType::Float16, mvp.v);
  Graphics::Internal::Assert(gLightmapMVP.valid, "On line: " "532" ", in file: " "./sample.cpp", "(e INvalid lightmap mvp?");
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

 for (u32 i = 0; i < 10; ++i) {
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

__attribute__ (( visibility( "default" ) )) extern "C" void StartSample(Graphics::Device* gfx) {
    Graphics::Dependencies deps;
    deps.Request = wasmGraphics_AllocateMem;
    deps.Release = wasmGraphics_ReleaseMem;
    Graphics::Internal::Assert(gfx != 0, "On line: " "55" ", in file: " "WasmSample.cpp", "Can't have null GFX");
    Initialize(&deps, gfx);
}

__attribute__ (( visibility( "default" ) )) extern "C" void UpdateSample(Graphics::Device* gfx, float dt) {
    Update(gfx, dt);
}

__attribute__ (( visibility( "default" ) )) extern "C" void RenderSample(Graphics::Device * gfx, int x, int y, int w, int h) {
    if (isFinishedInitializing) {
        Render(gfx, x, y, w, h);
    }
}
