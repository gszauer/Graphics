#pragma once

#ifndef ATLAS_U32
	#define ATLAS_U32
	typedef unsigned int u32;
	static_assert (sizeof(u32) == 4, "u32 should be defined as a 4 byte type");
#endif 

#ifndef ATLAS_F32
	#define ATLAS_F32
	typedef float f32;
	static_assert (sizeof(f32) == 4, "f32 should be defined as a 4 byte type");
#endif 

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

		inline BufferView(	u32 _numberOfComponents = 0, 
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

#if GRAPHICS_SHADERS_ENABLENAMES
		char* mUniformNames; // Might not be needed
		u32 mNumUniforms;    // Might not be needed

		char* mAttributeNames; // Might not be needed
		u32 mNumAttributes;    // Might not be needed
#endif
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
		bool mDepth;	// true
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