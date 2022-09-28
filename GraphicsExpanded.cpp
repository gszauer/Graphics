# 1 "GraphicsWASM.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 370 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "GraphicsWASM.cpp" 2
# 1 "./Graphics.h" 1




 typedef unsigned int u32;
 static_assert (sizeof(u32) == 4, "u32 should be defined as a 4 byte type");




 typedef float f32;
 static_assert (sizeof(f32) == 4, "f32 should be defined as a 4 byte type");


namespace Graphics {

 typedef void* (*fpRequest)(u32 bytes);
 typedef void (*fpRelease)(void* mem);

 struct Dependencies {
  fpRequest Request;
  fpRelease Release;

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
  Back = 1,
  Front = 2,
  FrontAndBack = 3
 };

 enum class FaceWind {
  CounterClockwise = 0,
  Clockwise = 1
 };

 enum class TextureFormat {
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

 struct Index {
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
  WrapMode wrapR;
  Filter min;
  Filter mip;
  Filter mag;

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
  u32 NumberOfComponents;
  u32 StrideInBytes;
  BufferType Type;
  u32 DataOffsetInBytes;

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





 class Texture {
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

  u32 mCachedMin;
  u32 mCachedMag;
  u32 mCachedS;
  u32 mCachedR;
  u32 mCachedT;

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
   Graphics::Texture* texture;
   u32 target;
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
# 328 "./Graphics.h"
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
# 371 "./Graphics.h"
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
# 399 "./Graphics.h"
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


  Internal::TextureUnit mBoundTextures[32];


  CullFace mFaceCulling;
  FaceWind mWindingOrder;


  bool mBlend;
  f32 mBlendColor[4];
  BlendFunction mBlendDstAlpha;
  BlendFunction mBlendDstRGB;
  BlendEquation mBlendEquationAlpha;
  BlendEquation mBlendEquationRGB;
  BlendFunction mBlendSrcAlpba;
  BlendFunction mBlendSrcRGB;


  bool mDepth;
  DepthFunc mDepthFunc;
  f32 mDepthRange[2];


  bool mScissor;
  u32 mScissorRect[4];


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

  void Bind(Shader* shader);
  inline void Bind(Shader& shader) {
   Bind(&shader);
  }


  void SetRenderTarget(FrameBuffer* frameBuffer);
  inline void SetRenderTarget(FrameBuffer& frameBuffer) {
   SetRenderTarget(&frameBuffer);
  }

  void Bind(Index& slot, UniformType type, void* data, u32 count = 1);
  void Bind(Index& uniformSlot, Texture& texture, Sampler& sampler);

  void Draw(const VertexLayout& attributes, DrawMode drawMode, u32 startIndex, u32 indexCount, u32 instanceCount = 1);
 public:

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



  void SetFaceVisibility(CullFace cull, FaceWind wind = FaceWind::CounterClockwise);



  void SetDepthState(bool enable, DepthFunc depthFunc, f32* depthRange = 0);
  inline void SetDepthState(DepthFunc depthFunc) {
   SetDepthState(true, depthFunc, 0);
  }
  inline void SetDepthState(bool enable) {
   SetDepthState(enable, DepthFunc::Less, 0);
  }



  void SetScissorState(bool enable, u32 x, u32 y, u32 w, u32 h);
  inline void SetScissorState(bool enable) {
   SetScissorState(enable, mScissorRect[0], mScissorRect[1], mScissorRect[2], mScissorRect[3]);
  }



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
# 2 "GraphicsWASM.cpp" 2
# 15 "GraphicsWASM.cpp"
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
extern "C" void wasmGraphics_GetScissorAndViewport(void* scissorPtr, void* viewPtr);
extern "C" int wasmGraphics_CompileShader(const void* vShader, int vShaderLen, const void* fShader, int fShaderLen);
# 155 "GraphicsWASM.cpp"
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
                                                                  ;

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

   return 0x81A6;
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
                                                            ;
   for (const char* i = vertexSource; *i != '\0'; ++i, ++vLen);
   const char* vp1 = vertexSource + 1;
                                                   ;

   int fLen = 0;
                                                                ;
   for (const char* i = fragmentSource; *i != '\0'; ++i, ++fLen);
                                                     ;

   result.program = wasmGraphics_CompileShader(vertexSource, vLen, fragmentSource, fLen);
   result.success = result.program != 0;
                                                             ;

   return result;
  }
 }
}


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
  else {
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
   depthState = 0;
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

  if (blendDstAlpha == blendDstRgb && blendSrcAlpha == blendSrcRGB) {
   wasmGraphics_ChangeGLBlendFuncSame(srcRgb, dstRgb);
  }
  else {
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

  if (blendEquationAlpha == blendEquationRgb) {
   wasmGraphics_ChangeGLBlendEquation(rgbEquation);
  }
  else {
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

 if (buffer->mAllocPrev != 0) {
  buffer->mAllocPrev->mAllocNext = buffer->mAllocNext;
  if (buffer->mAllocNext != 0) {
   buffer->mAllocNext->mAllocPrev = buffer->mAllocPrev;
  }
 }
 else {
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

 if (buff->mAllocPrev != 0) {
  buff->mAllocPrev->mAllocNext = buff->mAllocNext;
  if (buff->mAllocNext != 0) {
   buff->mAllocNext->mAllocPrev = buff->mAllocPrev;
  }
 }
 else {
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

 if (map->mAllocPrev != 0) {
  map->mAllocPrev->mAllocNext = map->mAllocNext;
  if (map->mAllocNext != 0) {
   map->mAllocNext->mAllocPrev = map->mAllocPrev;
  }
 }
 else {
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

 if (buff->mAllocPrev != 0) {
  buff->mAllocPrev->mAllocNext = buff->mAllocNext;
  if (buff->mAllocNext != 0) {
   buff->mAllocNext->mAllocPrev = buff->mAllocPrev;
  }
 }
 else {
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
                                                                  ;

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

 if (shader->mAllocPrev != 0) {
  shader->mAllocPrev->mAllocNext = shader->mAllocNext;
  if (shader->mAllocNext != 0) {
   shader->mAllocNext->mAllocPrev = shader->mAllocPrev;
  }
 }
 else {
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
                                                                                ;
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
  {
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
                                                      ;
                                                                                          ;
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


 u32 textureUnit = 33;
 u32 firstFree = 33;
 for (u32 i = 0; i < 32; ++i) {
  if (mBoundTextures[i].texture != 0) {
   if (mBoundTextures[i].index.valid && mBoundTextures[i].index.id == uniformSlot.id) {
    textureUnit = i;
                                                                                       ;
    break;
   }
  }
  else if (firstFree == 33) {
                                                                                  ;
   firstFree = i;
  }
 }
 if (textureUnit == 33) {
  textureUnit = firstFree;
  mBoundTextures[firstFree].index = uniformSlot;
                                                                              ;
  mBoundTextures[firstFree].target = target;
  mBoundTextures[firstFree].texture = &texture;
 }
                                                         ;


 GLenum enumTextureUnit = Internal::GetTextureUnit(textureUnit);

 bool updateSampler = false;

 if (texture.mCachedMin != min) {
  updateSampler = true;
  texture.mCachedMin = min;
 }
 if (texture.mCachedMag != mag) {
  updateSampler = true;
  texture.mCachedMag = mag;
 }


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


 outDevice.mFaceCulling = CullFace::Back;
 outDevice.mWindingOrder = FaceWind::CounterClockwise;


 outDevice.mDepth = true;
 outDevice.mDepthFunc = DepthFunc::Less;
 outDevice.mDepthRange[0] = 0.0f;
 outDevice.mDepthRange[1] = 1.0f;


 wasmGraphics_SetDefaultGLState();
 wasmGraphics_GetScissorAndViewport(outDevice.mScissorRect, outDevice.mViewportRect);
 outDevice.mScissor = false;

 return &outDevice;
}

void Graphics::Shutdown(Device& device) {
 device.Bind(0);
 wasmGraphics_BindVAO(0);

                                                                                   ;
                                                                                  ;
                                                                                 ;
                                                                                  ;
                                                                                       ;

 device.mPlatform.Request = 0;
 device.mPlatform.Release = 0;
}


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


void Graphics::FrameBuffer::AttachColor(Texture& color, u32 attachmentIndex) {
 if (attachmentIndex > 5) {
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

                                                                    ;
                                                                      ;

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
