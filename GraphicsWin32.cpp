#include "Graphics.h"
#include "gl.h"

#define graphics_xstr(a) graphics_str(a)
#define graphics_str(a) #a
#define graphics_location "On line: " graphics_xstr(__LINE__) ", in file: " __FILE__

#if _DEBUG
	#define GraphicsAssert(cond, msg) Graphics::Internal::Assert(cond, graphics_location, msg)
#else
	#define GraphicsAssert(c, m)
#endif

#ifndef ATLAS_I32
	#define ATLAS_I32
	typedef int i32;
	static_assert (sizeof(i32) == 4, "i32 should be defined as a 4 byte type");
#endif 

// Helpers
namespace Graphics {
	namespace Internal {
		void Assert(bool cond, const char* location, const char* msg) {
			if (!cond) {
				char* invalidAddress = (char*)0;
				*invalidAddress = '\0';
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

		inline u32 FindFirstAvailable(u32 mask) {
			for (u32 i = 0; i < 32; ++i) {
				if (!(mask & (1 << i))) {
					return i;
				}
			}

			GraphicsAssert(false, "Could not find available texture, all 32 slots are filled");
			return 33;
		}

		inline GLenum DepthFuncToEnum(DepthFunc f) {
			if (f == DepthFunc::Always) {
				return GL_ALWAYS;
			}
			else if (f == DepthFunc::Never) {
				return GL_NEVER;
			}
			else if (f == DepthFunc::Equal) {
				return GL_EQUAL;
			}
			else if (f == DepthFunc::LEqual) {
				return GL_LEQUAL;
			}
			else if (f == DepthFunc::Greater) {
				return GL_GREATER;
			}
			else if (f == DepthFunc::GEqual) {
				return GL_GEQUAL;
			}
			else if (f == DepthFunc::NotEqual) {
				return GL_NOTEQUAL;
			}

			return GL_LESS;
		}

		inline GLenum GetTextureUnit(u32 index) {
			GraphicsAssert(index <= 32, "Only supports up to GL_TEXTURE16");

			return GL_TEXTURE0 + index;
		}

		inline GLenum BlendEqToEnum(BlendEquation b) {
			if (b == BlendEquation::Subtract) {
				return GL_FUNC_SUBTRACT;
			}
			else if (b == BlendEquation::ReverseSubtract) {
				return GL_FUNC_REVERSE_SUBTRACT;
			}
			else if (b == BlendEquation::Min) {
				return GL_MIN;
			}
			else if (b == BlendEquation::Max) {
				return GL_MAX;
			}

			return GL_FUNC_ADD;
		}

		inline GLenum BlendfuncToEnum(BlendFunction b) {
			GLenum result = GL_ZERO;
			if (b == BlendFunction::One) {
				result = GL_ONE;
			}
			else if (b == BlendFunction::SrcColor) {
				result = GL_SRC_COLOR;
			}
			else if (b == BlendFunction::OneMinusSrcColor) {
				result = GL_ONE_MINUS_SRC_COLOR;
			}
			else if (b == BlendFunction::DstColor) {
				result = GL_DST_COLOR;
			}
			else if (b == BlendFunction::OneMinusDstColor) {
				result = GL_ONE_MINUS_DST_COLOR;
			}
			else if (b == BlendFunction::SrcAlpha) {
				result = GL_SRC_ALPHA;
			}
			else if (b == BlendFunction::OneMinusSrcAlpha) {
				result = GL_ONE_MINUS_SRC_ALPHA;
			}
			else if (b == BlendFunction::DstAlpha) {
				result = GL_DST_ALPHA;
			}
			else if (b == BlendFunction::OneMinusDstAlpha) {
				result = GL_ONE_MINUS_DST_ALPHA;
			}
			else if (b == BlendFunction::ConstColor) {
				result = GL_CONSTANT_COLOR;
			}
			else if (b == BlendFunction::OneMinusConstColor) {
				result = GL_ONE_MINUS_CONSTANT_COLOR;
			}
			else if (b == BlendFunction::ConstAlpha) {
				result = GL_CONSTANT_ALPHA;
			}
			else if (b == BlendFunction::OneMinusconstAlpha) {
				result = GL_ONE_MINUS_CONSTANT_ALPHA;
			}
			else if (b == BlendFunction::SrcAlphaSaturate) {
				result = GL_SRC_ALPHA_SATURATE;
			}
			return result;
		}

		inline GLenum DrawModeToEnum(DrawMode drawMode) {
			GLenum mode = GL_TRIANGLES;
			if (drawMode == DrawMode::Points) {
				mode = GL_POINTS;
			}
			else if (drawMode == DrawMode::Lines) {
				mode = GL_LINES;
			}
			else if (drawMode == DrawMode::LineStrip) {
				mode = GL_LINE_STRIP;
			}
			else if (drawMode == DrawMode::TriangleStrip) {
				mode = GL_TRIANGLE_STRIP;
			}
			else if (drawMode == DrawMode::TriangleFan) {
				mode = GL_TRIANGLE_FAN;
			}
			return mode;
		}

		inline GLenum TextureGetInternalFormatFromEnum(TextureFormat component) {
			if (component == TextureFormat::R8) {
				return GL_R8;
			}
			else if (component == TextureFormat::RG8) {
				return GL_RG8;
			}
			if (component == TextureFormat::RGB8) {
				return GL_RGB8;
			}
			if (component == TextureFormat::RGBA8) {
				return GL_RGBA8;
			}

			if (component == TextureFormat::R32F) {
				return GL_R32F;
			}
			else if (component == TextureFormat::RG32F) {
				return GL_RG32F;
			}
			if (component == TextureFormat::RGB32F) {
				return GL_RGB32F;
			}
			if (component == TextureFormat::RGBA32F) {
				return GL_RGBA32F;
			}

			return GL_DEPTH_COMPONENT; // Default to depth i guess
		}

		inline TextureFormatResult TextureGetDataFormatFromEnum(TextureFormat component) {
			GLenum dataFormat = GL_DEPTH_COMPONENT;
			GLenum dataType = GL_UNSIGNED_BYTE; 

			if (component == TextureFormat::R8) {
				dataFormat = GL_RED;
				dataType = GL_UNSIGNED_BYTE;
			}
			else if (component == TextureFormat::RG8) {
				dataFormat = GL_RG;
				dataType = GL_UNSIGNED_BYTE;
			}
			if (component == TextureFormat::RGB8) {
				dataFormat = GL_RGB;
				dataType = GL_UNSIGNED_BYTE;
			}
			if (component == TextureFormat::RGBA8) {
				dataFormat = GL_RGBA;
				dataType = GL_UNSIGNED_BYTE;
			}

			if (component == TextureFormat::R32F) {
				dataFormat = GL_RED;
				dataType = GL_FLOAT;
			}
			else if (component == TextureFormat::RG32F) {
				dataFormat = GL_RG;
				dataType = GL_FLOAT;
			}
			if (component == TextureFormat::RGB32F) {
				dataFormat = GL_RGB;
				dataType = GL_FLOAT;
			}
			if (component == TextureFormat::RGBA32F) {
				dataFormat = GL_RGBA;
				dataType = GL_FLOAT;
			}
			
			TextureFormatResult result;
			result.dataFormat = dataFormat;
			result.dataType = dataType;
			return result;
		}

		inline GLenum BufferTypeToEnum(BufferType bufferType) {
			GLenum type = GL_FLOAT;

			if (bufferType == BufferType::Int8) {
				type = GL_BYTE;
			}
			else if (bufferType == BufferType::UInt8) {
				type = GL_UNSIGNED_BYTE;
			}
			else if (bufferType == BufferType::Int16) {
				type = GL_SHORT;
			}
			else if (bufferType == BufferType::UInt16) {
				type = GL_UNSIGNED_SHORT;
			}
			else if (bufferType == BufferType::Int32) {
				type = GL_INT;
			}
			else if (bufferType == BufferType::UInt32) {
				type = GL_UNSIGNED_INT;
			}

			return type;
		}

#if GRAPHICS_SHADERS_ENABLENAMES
		struct ShaderBytesNeededResult {
			u32 bytesNeeded;
			u32 numberOfThing;
		};

		ShaderBytesNeededResult NumBytesNeededForAttributeNames(unsigned int handle) {
			int count = -1;
			int length;
			char name[128];
			int size;
			GLenum type;

			ShaderBytesNeededResult result;
			//glUseProgram(handle);
			glGetProgramiv(handle, GL_ACTIVE_ATTRIBUTES, &count);
			result.numberOfThing = count;

			result.bytesNeeded = 0;

			for (int i = 0; i < count; ++i) {
				memset(name, 0, sizeof(char) * 128);
				glGetActiveAttrib(handle, (GLuint)i, 128, &length, &size, &type, name);
				GraphicsAssert(length < 128, "Name too long");
				result.bytesNeeded += length + 1;
				//int attrib = glGetAttribLocation(handle, name);
			}

			//glUseProgram(0);

			result.bytesNeeded += 1;
			return result;
		}

		ShaderBytesNeededResult NumBytesNeededForUniformNames(unsigned int handle) {
			int count = -1;
			int length;
			char name[128];
			int size;
			GLenum type;
			char testName[256];

			ShaderBytesNeededResult result;
			//glUseProgram(handle);
			glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);
			result.numberOfThing = count;
			result.bytesNeeded = 0;

			for (int i = 0; i < count; ++i) {
				memset(name, 0, sizeof(char) * 128);
				glGetActiveUniform(handle, (GLuint)i, 128, &length, &size, &type, name);
				GraphicsAssert(length < 128, "Name too long");
				//int uniform = glGetUniformLocation(handle, name);
				result.bytesNeeded += length + 1;
			}

			//glUseProgram(0);
			result.bytesNeeded += 1;
			return result;
		}
#endif

		ShaderCompileResult CompileOpenGLShader(const char* vertexSource, const char* fragmentSource, Dependencies* platform) {
			ShaderCompileResult result;
			result.success = true;
			result.program = 0;
			int success = 1;

			// compile vertex shader
			u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &vertexSource, NULL);
			glCompileShader(vertexShader);
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				result.success = false;
#if _DEBUG
				char infoLog[512];
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
				GraphicsAssert(false, infoLog);
#endif
			}

			// compile fragment shader
			u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
			glCompileShader(fragmentShader);
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				result.success = false;
#if _DEBUG
				char infoLog[512];
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
				GraphicsAssert(false, infoLog);
#endif
			}

			// Link shaders into program
			u32 shaderProgram = glCreateProgram();
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				result.success = false;
#if _DEBUG
				char infoLog[512];
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
				GraphicsAssert(false, infoLog);
#endif
			}
			else {
				result.program = shaderProgram;
			}

			// Delete shaders
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

#if GRAPHICS_SHADERS_ENABLENAMES
			int length;
			int size;
			GLenum type;

			ShaderBytesNeededResult attribNameInfo = Internal::NumBytesNeededForAttributeNames(result.program);
			result.attributes = (char*)platform->Request(attribNameInfo.bytesNeeded);
			result.numAttribs = attribNameInfo.numberOfThing;
			char* iter = result.attributes;
			for (u32 i = 0; i < attribNameInfo.numberOfThing; ++i) {
				char* name = iter;
				glGetActiveAttrib(result.program, i, 128, &length, &size, &type, name);
				iter += length;
				*iter = (i + 1 == attribNameInfo.numberOfThing)? '\0' : '\n';
				iter += 1;
			}
			*iter = '\0';

			ShaderBytesNeededResult uniformNameInfo = Internal::NumBytesNeededForUniformNames(result.program);
			result.uniforms = (char*)platform->Request(uniformNameInfo.bytesNeeded);
			result.numUniforms = uniformNameInfo.numberOfThing;
			iter = result.uniforms;
			for (u32 i = 0; i < uniformNameInfo.numberOfThing; ++i) {
				char* name = iter;
				glGetActiveUniform(result.program, i, 128, &length, &size, &type, name);
				iter += length;
				*iter = (i + 1 == uniformNameInfo.numberOfThing) ? '\0' : '\n';
				iter += 1;
			}
			*iter = '\0';
#endif

			return result;
		}
	}
}

/// Texture
void Graphics::Texture::SetPCM(bool pcm) {
	GLenum attachTarget = GL_TEXTURE_2D;
	if (mIsCubeMap) {
		attachTarget = GL_TEXTURE_CUBE_MAP;
	}

	glBindTexture(attachTarget, mId);
	if (pcm) {
		glTexParameteri(attachTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(attachTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	else {
		glTexParameteri(attachTarget, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(attachTarget, GL_TEXTURE_COMPARE_FUNC, GL_NONE);
	}
	glBindTexture(attachTarget, 0);
}

void Graphics::Texture::Set(void* data, TextureFormat dataFormat, u32 width, u32 height, bool genMipMaps) {
	glBindTexture(GL_TEXTURE_2D, mId); 

	GLenum internalFormat = Internal::TextureGetInternalFormatFromEnum(mInternalFormat);
	Internal::TextureFormatResult f = Internal::TextureGetDataFormatFromEnum(dataFormat);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, data);
	if (genMipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	mWidth = width;
	mHeight = height;
	mIsCubeMap = false;
	mIsMipMapped = genMipMaps;

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Graphics::Texture::SetCubemap(void* rightData, void* leftData, void* topData, void* bottomData, void* backData, void* frontData, 
	u32 width, u32 height, TextureFormat texFormat, bool genMipMaps) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, mId);

	GLenum internalFormat = Internal::TextureGetInternalFormatFromEnum(mInternalFormat);
	Internal::TextureFormatResult f = Internal::TextureGetDataFormatFromEnum(texFormat);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, rightData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, leftData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, topData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, bottomData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, backData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, width, height, 0, f.dataFormat, f.dataType, frontData);

	if (genMipMaps) {
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	mWidth = width;
	mHeight = height;
	mIsCubeMap = true;
	mIsMipMapped = genMipMaps;

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/// Device
void Graphics::Device::SetFaceVisibility(CullFace cull, FaceWind wind) {
	if (mFaceCulling != cull) {
		if (cull == CullFace::Back) {
			if (mFaceCulling == CullFace::Off) {
				glEnable(GL_CULL_FACE);
			}
			glCullFace(GL_BACK);
		}
		else if (cull == CullFace::Front) {
			if (mFaceCulling == CullFace::Off) {
				glEnable(GL_CULL_FACE);
			}
			glCullFace(GL_FRONT);
		}
		else if (cull == CullFace::FrontAndBack) {
			if (mFaceCulling == CullFace::Off) {
				glEnable(GL_CULL_FACE);
			}
			glCullFace(GL_FRONT_AND_BACK);
		}
		else { // Off
			if (mFaceCulling != CullFace::Off) {
				glDisable(GL_CULL_FACE);
			}
		}
		mFaceCulling = cull;
	}

	if (mWindingOrder != wind) {
		if (wind == FaceWind::CounterClockwise) {
			glFrontFace(GL_CCW);
		}
		else {
			glFrontFace(GL_CW);
		}
		mWindingOrder = wind;
	}
}

void Graphics::Device::SetDepthState(bool enable, DepthFunc depthFunc, f32* depthRange) {
	if (mDepth != enable) {
		if (enable) {
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
		mDepth = enable;
	}

	if (mDepthFunc != depthFunc) {
		GLenum func = Internal::DepthFuncToEnum(depthFunc);
		glDepthFunc(func);
		mDepthFunc = depthFunc;
	}

	if (depthRange != 0) {
		glDepthRange(depthRange[0], depthRange[1]);
		mDepthRange[0] = depthRange[0];
		mDepthRange[1] = depthRange[1];
	}
}

void Graphics::Device::SetBlendState(bool blend, f32* blendColor,
	BlendFunction blendDstRgb, BlendFunction blendDstAlpha,
	BlendEquation blendEquationRgb, BlendEquation blendEquationAlpha,
	BlendFunction blendSrcRGB, BlendFunction blendSrcAlpha) {

	if (blend != mBlend) {
		if (blend) {
			glEnable(GL_BLEND);
		}
		else {
			glDisable(GL_BLEND);
		}
		mBlend = blend;
	}

	if (blendColor != 0) {
		glBlendColor(blendColor[0], blendColor[1], blendColor[2], blendColor[3]);
		mBlendColor[0] = blendColor[0];
		mBlendColor[1] = blendColor[1];
		mBlendColor[2] = blendColor[2];
		mBlendColor[3] = blendColor[3];
	}

	if (mBlendDstAlpha != blendDstAlpha || mBlendDstRGB != blendDstRgb ||
		mBlendSrcAlpba != blendSrcAlpha || mBlendSrcRGB != blendSrcRGB) {
		GLenum srcAlpha = Internal::BlendfuncToEnum(blendSrcAlpha);
		GLenum srcRgb = Internal::BlendfuncToEnum(blendSrcRGB);
		GLenum dstAlpha = Internal::BlendfuncToEnum(blendDstAlpha);
		GLenum dstRgb = Internal::BlendfuncToEnum(blendDstRgb);

		if (blendDstAlpha == blendDstRgb && blendSrcAlpha == blendSrcRGB) { // Same
			glBlendFunc(srcRgb, dstRgb);

		}
		else { // Seperate
			glBlendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha);
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
			glBlendEquation(rgbEquation);
		}
		else { // Seperate
			glBlendEquationSeparate(rgbEquation, alphaEquation);
		}

		mBlendEquationAlpha = blendEquationAlpha;
		mBlendEquationRGB = blendEquationRgb;
	}
}

void Graphics::Device::Clear(f32 r, f32 g, f32 b, f32 depth) {
	glClearColor(r, g, b, 1.0f);
	glClearDepth(depth);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::Device::Clear(bool color, bool depth) {
	if (color && !depth) {
		glClear(GL_COLOR_BUFFER_BIT);
	}
	else if (!color && depth) {
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else if (color && depth) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void Graphics::Device::WriteMask(bool r, bool g, bool b, bool a, bool depth) {
	glColorMask(r, g, b, a);
	glDepthMask(depth);
}

void Graphics::Device::Clear(f32 r, f32 g, f32 b) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::Device::Clear(f32 depth) {
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Graphics::Device::SetViewport(u32 x, u32 y, u32 w, u32 h) {
	if (x != mViewportRect[0] || y != mViewportRect[1] ||
		w != mViewportRect[2] || h != mViewportRect[3]) {
		glViewport(x, y, w, h);
		mViewportRect[0] = x;
		mViewportRect[1] = y;
		mViewportRect[2] = w;
		mViewportRect[3] = h;
	}
}

void Graphics::Device::SetScissorState(bool enable, u32 x, u32 y, u32 w, u32 h) {
	if (enable != mScissor) {
		if (enable) {
			glEnable(GL_SCISSOR_TEST);
		}
		else {
			glDisable(GL_SCISSOR_TEST);
		}

		mScissor = enable;
	}

	if (x != mScissorRect[0] || y != mScissorRect[1] || w != mScissorRect[2] || h != mScissorRect[3]) {
		glScissor(x, y, w, h);

		mScissorRect[0] = x;
		mScissorRect[1] = y;
		mScissorRect[2] = w;
		mScissorRect[3] = h;
	}
}

Graphics::FrameBuffer* Graphics::Device::CreateFrameBuffer() {
	Graphics::FrameBuffer* result = (Graphics::FrameBuffer*)mPlatform.Request(sizeof(Graphics::FrameBuffer));
	glGenFramebuffers(1, &result->mId);
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
	result->mReadBufferConfig = GL_FRONT;

	return result;
}

void Graphics::Device::Destroy(FrameBuffer* buffer) {
	if (mBoundFrameBuffer == buffer->mId) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		mBoundFrameBuffer = 0;
	}
	glDeleteFramebuffers(1, &buffer->mId);

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
	glGenBuffers(1, &result->mId);
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
	glGenBuffers(1, &result->mId);
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
	glDeleteBuffers(1, &buff->mId);

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

	glGenVertexArrays(1, &result->mId);
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
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &map->mId);
	
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

	glGenTextures(1, &result->mId);
	result->mWidth = 0;
	result->mHeight = 0;
	result->mInternalFormat = format;
	result->mIsMipMapped = false;
	result->mIsCubeMap = false;
	result->mUserData = 0;

	result->mCachedMin = GL_NEAREST_MIPMAP_LINEAR;
	result->mCachedMag = GL_LINEAR;

	result->mCachedS = GL_REPEAT;
	result->mCachedR = GL_REPEAT;
	result->mCachedT = GL_REPEAT;

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
	glDeleteTextures(1, &buff->mId);

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
	GraphicsAssert(compileStatus.success, "Failed to compile shader");

	if (compileStatus.success) {
		result = (Graphics::Shader*)mPlatform.Request(sizeof(Graphics::Shader));
		result->mProgram = compileStatus.program;
	}

#if GRAPHICS_SHADERS_ENABLENAMES
	result->mUniformNames = compileStatus.uniforms;
	result->mNumUniforms = compileStatus.numUniforms;
	result->mAttributeNames = compileStatus.attributes;
	result->mNumAttributes = compileStatus.numAttribs;
#endif

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
	if (mBoundProgram == shader->mProgram) {
		glUseProgram(0);
		mBoundProgram = 0;
	}
	glDeleteProgram(shader->mProgram);
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
#if GRAPHICS_SHADERS_ENABLENAMES
	mPlatform.Release(shader->mUniformNames);
	mPlatform.Release(shader->mAttributeNames);
#endif
	mPlatform.Release(shader);
}

void Graphics::Device::SetRenderTarget(FrameBuffer* frameBuffer) {
	const u32 attachments[8] = { 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,
	};
	if (frameBuffer != 0) {
		if (mBoundFrameBuffer != frameBuffer->mId) {
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->mId);
			mBoundFrameBuffer = frameBuffer->mId;
		}
		u32 numAttachments = frameBuffer->TargetCount();
		if (numAttachments == 0) {
			glDrawBuffer(GL_NONE);
			if (frameBuffer->mReadBufferConfig != GL_NONE) {
				glReadBuffer(GL_NONE);
				frameBuffer->mReadBufferConfig = GL_NONE;
			}
		}
		else {
			GraphicsAssert(numAttachments <= 8, "Only supports up to 8 color attachments");
			glDrawBuffers(numAttachments, attachments);
			if (frameBuffer->mReadBufferConfig != GL_FRONT) {
				glReadBuffer(GL_FRONT);
				frameBuffer->mReadBufferConfig = GL_FRONT;
			}
		}
		
	}
	else if (mBoundFrameBuffer != 0) {
		const u32 back_attach[1] = {GL_BACK};
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		mBoundFrameBuffer = 0;
		glDrawBuffers(1, back_attach);
	}
}


void Graphics::Device::Bind(Shader* shader) {
	u32 program = 0;
	if (shader != 0) {
		program = shader->mProgram;
	}
	
	if (mBoundProgram != program) {
		glUseProgram(program);
		mBoundProgram = program;

		{  // Unbind any previously bound textures
			for (u32 i = 0; i < 32; ++i) {
				if (mBoundTextures[i].texture != 0) {
					glActiveTexture(Internal::GetTextureUnit(i));
					glBindTexture(mBoundTextures[i].target, 0);
					mBoundTextures[i].texture = 0;
					mBoundTextures[i].target = 0;
					mBoundTextures[i].index.id = 0;
					mBoundTextures[i].index.valid = false;

				}
			}
			glActiveTexture(GL_TEXTURE0);
		}
	}
}

void Graphics::Device::Bind(Index& slot, UniformType type, void* data, u32 count) {
	if (type == UniformType::Int1) {
		glUniform1iv(slot.id, count, (const GLint*)data);
	}
	else if (type == UniformType::Int2) {
		glUniform2iv(slot.id, count, (const GLint*)data);
	}
	else if (type == UniformType::Int3) {
		glUniform3iv(slot.id, count, (const GLint*)data);
	}
	else if (type == UniformType::Int4) {
		glUniform4iv(slot.id, count, (const GLint*)data);
	}
	else if (type == UniformType::Float1) {
		glUniform1fv(slot.id, count, (GLfloat*)data);
	}
	else if (type == UniformType::Float2) {
		glUniform2fv(slot.id, count, (GLfloat*)data);
	}
	else if (type == UniformType::Float3) {
		glUniform3fv(slot.id, count, (GLfloat*)data);
	}
	else if (type == UniformType::Float4) {
		glUniform4fv(slot.id, count, (GLfloat*)data);
	}
	else if (type == UniformType::Float9) {
		glUniformMatrix3fv(slot.id, count, GL_FALSE, (GLfloat*)data);
	}
	else if (type == UniformType::Float16) {
		glUniformMatrix4fv(slot.id, count, GL_FALSE, (GLfloat*)data);
	}
}

void Graphics::Device::Bind(Index& uniformSlot, Texture& texture, Sampler& sampler) {
	GLenum min = GL_NEAREST;
	GLenum mag = GL_LINEAR;

	if (texture.mIsMipMapped) {
		if (sampler.min == Filter::Nearest) {
			if (sampler.mip == Filter::Nearest) {
				min = GL_NEAREST_MIPMAP_NEAREST;
			}
			else {
				min = GL_NEAREST_MIPMAP_LINEAR;
			}
		}
		else {
			if (sampler.mip == Filter::Nearest) {
				min = GL_LINEAR_MIPMAP_NEAREST;
			}
			else {
				min = GL_LINEAR_MIPMAP_LINEAR;
			}
		}
	}
	else {
		if (sampler.min == Filter::Linear) {
			min = GL_LINEAR;
		}
	}

	if (sampler.mag == Filter::Nearest) {
		mag = GL_NEAREST;
	}

	GLenum wrapS = GL_REPEAT;
	GLenum wrapT = GL_REPEAT;
	GLenum wrapR = GL_REPEAT;

	if (sampler.wrapS == WrapMode::Clamp) {
		wrapS = GL_CLAMP_TO_EDGE;
	}

	if (sampler.wrapT == WrapMode::Clamp) {
		wrapT = GL_CLAMP_TO_EDGE;
	}

	if (sampler.wrapR == WrapMode::Clamp) {
		wrapR = GL_CLAMP_TO_EDGE;
	}

	GLenum target = GL_TEXTURE_2D;
	if (texture.mIsCubeMap) {
		target = GL_TEXTURE_CUBE_MAP;
	}

	// Find texture unit
	u32 textureUnit = 33;
	u32 firstFree = 33;
	for (u32 i = 0; i < 32; ++i) {
		if (mBoundTextures[i].texture != 0) { // Something is bound
			if (mBoundTextures[i].index.id == uniformSlot.id) { // Re-use
				textureUnit = i;
				GraphicsAssert(target == mBoundTextures[i].target, "Binding invalid texture types");
				break;
			}
		}
		else if (firstFree == 33) {
			firstFree = i;
		}
	}
	if (textureUnit == 33) {
		textureUnit = firstFree;
		mBoundTextures[firstFree].index = uniformSlot;
		mBoundTextures[firstFree].target = target;
		mBoundTextures[firstFree].texture = &texture;
	}
	GraphicsAssert(textureUnit < 33, "Invalid texture unit");

	glActiveTexture(Internal::GetTextureUnit(textureUnit));
	glBindTexture(target, texture.mId);

	// Set min and mag filter
	if (texture.mCachedMin != min) {
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
		texture.mCachedMin = min;
	}
	if (texture.mCachedMag != mag) {
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
		texture.mCachedMag = mag;
	}

	// Set Wrap Mode
	if (texture.mCachedS != wrapS) {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
		texture.mCachedS = wrapS;
	}
	if (texture.mCachedT != wrapT) {
		glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
		texture.mCachedT = wrapT;
	}
	if (texture.mIsCubeMap) {
		if (texture.mCachedR != wrapR) {
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
			texture.mCachedR = wrapR;
		}
	}

	glUniform1i(uniformSlot.id, textureUnit);
}

void Graphics::Device::Draw(const VertexLayout& attributes, DrawMode drawMode, u32 startIndex, u32 indexCount, u32 instanceCount) {
	glBindVertexArray(attributes.mId);
	if (attributes.mHasIndexBuffer) {
		GLenum type = Internal::BufferTypeToEnum(attributes.mIndexBufferType);
		if (instanceCount <= 1) {
			glDrawElements(Internal::DrawModeToEnum(drawMode), indexCount, type, (void*)startIndex);
		}
		else {
			glDrawElementsInstanced(Internal::DrawModeToEnum(drawMode), indexCount, type, (void*)startIndex, instanceCount);
		}
	}
	else {
		if (instanceCount <= 1) {
			glDrawArrays(Internal::DrawModeToEnum(drawMode), startIndex, indexCount);
		}
		else {
			glDrawArraysInstanced(Internal::DrawModeToEnum(drawMode), startIndex, indexCount, instanceCount);
		}
	}
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
	glDisable(GL_BLEND);
	outDevice.mBlend = false;

	glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
	outDevice.mBlendColor[0] = 0.0f;
	outDevice.mBlendColor[1] = 0.0f;
	outDevice.mBlendColor[2] = 0.0f;
	outDevice.mBlendColor[3] = 0.0f;

	glBlendFunc(GL_ONE, GL_ZERO);
	outDevice.mBlendSrcAlpba = BlendFunction::One;
	outDevice.mBlendSrcRGB = BlendFunction::One;
	outDevice.mBlendDstAlpha = BlendFunction::Zero;
	outDevice.mBlendDstRGB = BlendFunction::Zero;

	glBlendEquation(GL_FUNC_ADD);
	outDevice.mBlendEquationAlpha = BlendEquation::Add;
	outDevice.mBlendEquationRGB = BlendEquation::Add;

	// Cull state
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	outDevice.mFaceCulling = CullFace::Back;
	outDevice.mWindingOrder = FaceWind::CounterClockwise;
	
	// Depth state
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthRange(0.0f, 1.0f);
	outDevice.mDepth = true;
	outDevice.mDepthFunc = DepthFunc::Less;
	outDevice.mDepthRange[0] = 0.0f;
	outDevice.mDepthRange[1] = 1.0f;

	// Scissor state
	glDisable(GL_SCISSOR_TEST);
	glScissor(0, 0, 800, 600);

	GLint params[4];
	outDevice.mScissor = false;
	glGetIntegerv(GL_SCISSOR_BOX, params);
	outDevice.mScissorRect[0] = params[0];
	outDevice.mScissorRect[1] = params[1];
	outDevice.mScissorRect[2] = params[2];
	outDevice.mScissorRect[3] = params[3];

	glGetIntegerv(GL_VIEWPORT, params);
	outDevice.mViewportRect[0] = params[0];
	outDevice.mViewportRect[1] = params[1];
	outDevice.mViewportRect[2] = params[2];
	outDevice.mViewportRect[3] = params[3];

	// Make the default back buffer pretty?
	glEnable(GL_MULTISAMPLE); 

	return &outDevice;
}

void Graphics::Shutdown(Device& device) {
	device.Bind(0);
	glBindVertexArray(0);

	GraphicsAssert(device.mAllocatedTextures == 0, "Not all memory has been released");
	GraphicsAssert(device.mAllocatedBuffers == 0, "Not all memory has been released");
	GraphicsAssert(device.mAllocatedStates == 0, "Not all memory has been released");
	GraphicsAssert(device.mAllocatedShaders == 0, "Not all memory has been released");
	GraphicsAssert(device.mAllocatedFrameBuffers == 0, "Not all memory has been released");

	device.mPlatform.Request = 0;
	device.mPlatform.Release = 0;
}

// Pipeline state
void Graphics::VertexLayout::Set(const Index& index, const Buffer& buffer, const BufferView& view, u32 instanceDivisor) {
	GLenum type = Internal::BufferTypeToEnum(view.Type);
	glBindVertexArray(mId);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.mId);
	glVertexAttribPointer(index.id, view.NumberOfComponents, type, GL_FALSE, view.StrideInBytes, (void*)view.DataOffsetInBytes);
	glEnableVertexAttribArray(index.id);
	// glVertexAttribPointer: The bound buffer is now the source of the current attribute
	glVertexAttribDivisor(index.id, instanceDivisor);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Graphics::VertexLayout::Set(const Buffer& indexBuffer, const BufferType& indexType) {
	glBindVertexArray(mId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.mId);
	mHasIndexBuffer = true;
	mIndexBufferType = indexType;
	// assert on type. Index can only be u8, u16, u32 or signed i guess
	glBindVertexArray(0);
}

void Graphics::VertexLayout::Reset() {
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &mId);
	glGenVertexArrays(1, &mId);
	mHasIndexBuffer = false;
}

// Shader

Graphics::Index Graphics::Shader::GetAttribute(const char* name) {
	GLint location = glGetAttribLocation(mProgram, name);
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
	GLint location = glGetUniformLocation(mProgram, name);
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
	if (mIndexBuffer) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, arraySizeInBytes, inputArray, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, mId);
		glBufferData(GL_ARRAY_BUFFER, arraySizeInBytes, inputArray, _static? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Graphics::Buffer::Reset() {
	glDeleteBuffers(1, &mId);
	glGenBuffers(1, &mId);
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

	GLenum attachTarget = GL_TEXTURE_2D;
	if (color.mIsCubeMap) {
		attachTarget = GL_TEXTURE_CUBE_MAP;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, mId);
	
	glBindTexture(attachTarget, color.mId);
	if (color.mCachedMin != GL_LINEAR) {
		glTexParameteri(attachTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		color.mCachedMin = GL_LINEAR;
	}
	if (color.mCachedMag != GL_LINEAR) {
		glTexParameteri(attachTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		color.mCachedMag = GL_LINEAR;
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, attachTarget, color.mId, 0);
	glBindTexture(attachTarget, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mColor[attachmentIndex] = &color;
	mOwner->mBoundFrameBuffer = 0;
}

void Graphics::FrameBuffer::AttachDepth(Texture& depth, bool pcm) {
	GLenum attachTarget = GL_TEXTURE_2D;
	if (depth.mIsCubeMap) {
		attachTarget = GL_TEXTURE_CUBE_MAP;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, mId);
	
	glBindTexture(attachTarget, depth.mId);

	if (pcm) {
		glTexParameteri(attachTarget, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(attachTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		// Could call texture->SetPCM, but that would unbind the texture
	}

	if (depth.mCachedMin != GL_LINEAR) {
		glTexParameteri(attachTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		depth.mCachedMin = GL_LINEAR;
	}
	if (depth.mCachedMag != GL_LINEAR) {
		glTexParameteri(attachTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		depth.mCachedMag = GL_LINEAR;
	}
	glBindTexture(attachTarget, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, attachTarget, depth.mId, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	mDepth = &depth;
	mOwner->mBoundFrameBuffer = 0;
}

bool Graphics::FrameBuffer::IsValid() {
	glBindFramebuffer(GL_FRAMEBUFFER, mId);
	bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mOwner->mBoundFrameBuffer = 0;
	return result;
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

	GLbitfield mask = 0;
	if (hasColor && targetHasColor && color) {
		mask |= GL_COLOR_BUFFER_BIT;
	}
	if (hasDepth && targetHasDepth && depth) {
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	GLenum gl_filter = GL_NEAREST;
	if (filter == Filter::Linear) {
		gl_filter = GL_LINEAR;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, mId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetId);
	glBlitFramebuffer(x0, y0, x1, y1, x2, y2, x3, y3, mask, gl_filter);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mOwner->mBoundFrameBuffer = 0;
}

void Graphics::FrameBuffer::ResolveTo(FrameBuffer* target, Filter filter, bool color, bool depth) {
	u32 width = GetWidth();
	u32 height = GetHeight();

	GraphicsAssert(GetWidth() == target->GetWidth(), "Invalid resolve");
	GraphicsAssert(GetHeight() == target->GetHeight(), "Invalid resolve");
	
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