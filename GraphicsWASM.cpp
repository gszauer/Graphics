#include "Graphics.h"

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

typedef int GLenum;

extern "C" void wasmGraphics_Log(const char* loc, int locLen, const char* msg, int msgLen);
extern "C" void wasmGraphics_SetTexturePCM(int glTextureId, int glTextureAttachTarget, int glCompareMode, int glCompareFunc);
extern "C" void wasmGraphics_TextureSetData(int glTextureId, int glInternalFormat, int width, int height, int glDataFormat, int glDataFormatType, void* data, bool genMipMaps);
//extern "C" void wasmGraphics_TextureSetCubemap(int glTextureId, int glInternalFormat, int width, int height, int glDataFormat, int glDataType, void* rightData, void* leftData, void* topData, void* bottomData, void* backData, void* frontData, bool genMipMaps);
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
#define GL_NONE							0
#define GL_ZERO							0
#define GL_ONE							1
#define GL_ALWAYS						0x0207
#define GL_NEVER						0x0200
#define GL_EQUAL						0x0202
#define GL_LEQUAL						0x0203
#define GL_GREATER						0x0204
#define GL_GEQUAL						0x0206
#define GL_NOTEQUAL						0x0205
#define GL_LESS							0x0201
#define GL_TEXTURE0						0x84C0
#define GL_FUNC_SUBTRACT				0x800A
#define GL_FUNC_REVERSE_SUBTRACT		0x800B
#define GL_MIN							0x8007
#define GL_MAX							0x8008
#define GL_FUNC_ADD						0x8006
#define GL_SRC_COLOR					0x0300
#define GL_ONE_MINUS_SRC_COLOR			0x0301
#define GL_DST_COLOR					0x0306
#define GL_ONE_MINUS_DST_COLOR			0x0307
#define GL_SRC_ALPHA					0x0302
#define GL_ONE_MINUS_SRC_ALPHA			0x0303
#define GL_DST_ALPHA					0x0304
#define GL_ONE_MINUS_DST_ALPHA			0x0305
#define GL_CONSTANT_COLOR				0x8001
#define GL_ONE_MINUS_CONSTANT_COLOR		0x8002
#define GL_CONSTANT_ALPHA				0x8003
#define GL_ONE_MINUS_CONSTANT_ALPHA		0x8004
#define GL_SRC_ALPHA_SATURATE			0x0308
#define GL_TRIANGLES					0x0004
#define GL_POINTS						0x0000
#define GL_LINES						0x0001
#define GL_LINE_STRIP					0x0003
#define GL_TRIANGLE_STRIP				0x0005
#define GL_TRIANGLE_FAN					0x0006
#define GL_R8							0x8229
#define GL_RG8							0x822B
#define GL_RGB8							0x8051
#define GL_RGBA8						0x8058
#define GL_RG							0x8227
#define GL_R32F							0x822E
#define GL_RG32F						0x8230
#define GL_RGB32F						0x8815
#define GL_RGBA32F						0x8814
#define GL_DEPTH_COMPONENT24			0x81A6
#define GL_DEPTH_COMPONENT32F			0x8CAC
#define GL_DEPTH_COMPONENT16			0x81A5
#define GL_DEPTH_COMPONENT				0x1902
#define GL_UNSIGNED_BYTE				0x1401
#define GL_RGB							0x1907
#define GL_RGBA							0x1908
#define GL_RED							0x1903
#define GL_FLOAT						0x1406
#define GL_BYTE							0x1400
#define GL_SHORT						0x1402
#define GL_INT							0x1404
#define GL_UNSIGNED_INT					0x1405
#define GL_UNSIGNED_SHORT				0x1403	
#define GL_TEXTURE_2D					0x0DE1
#define GL_COMPARE_REF_TO_TEXTURE		0x884E
#define GL_BACK							0x0405
#define GL_CCW							0x0901
#define GL_FRONT						0x0404
#define GL_FRONT_AND_BACK				0x0408
#define GL_CW							0x0900
#define GL_DEPTH_TEST					0x0B71
#define GL_NEAREST_MIPMAP_LINEAR		0x2702
#define GL_LINEAR						0x2601
#define GL_REPEAT						0x2901
#define GL_COLOR_ATTACHMENT0			0x8CE0
#define GL_COLOR_ATTACHMENT1			0x8CE1
#define GL_COLOR_ATTACHMENT2			0x8CE2
#define GL_COLOR_ATTACHMENT3			0x8CE3
#define GL_COLOR_ATTACHMENT4			0x8CE4
#define GL_COLOR_ATTACHMENT5			0x8CE5
#define GL_COLOR_ATTACHMENT6			0x8CE6
#define GL_COLOR_ATTACHMENT7			0x8CE7
#define GL_NEAREST						0x2600
#define GL_NEAREST_MIPMAP_NEAREST		0x2700
#define GL_LINEAR_MIPMAP_NEAREST		0x2701
#define GL_LINEAR_MIPMAP_LINEAR			0x2703
#define GL_CLAMP_TO_EDGE				0x812F

#define GL_FLOAT_32_UNSIGNED_INT_24_8_REV 0x8DAD
#define GL_DEPTH_STENCIL				0x84F9
#define GL_DEPTH32F_STENCIL8			0x8CAD

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

			return GL_DEPTH_COMPONENT32F; // Default to depth i guess
		}

		// https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexImage2D.xhtml
		inline TextureFormatResult TextureGetDataFormatFromEnum(TextureFormat component) {
			GLenum dataFormat = GL_DEPTH_COMPONENT;
			GLenum dataType = GL_FLOAT; 

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

		ShaderCompileResult CompileOpenGLShader(const char* vertexSource, const char* fragmentSource, Dependencies* platform) {
			ShaderCompileResult result;
			result.program = 0;

			int vLen = 0;
			GraphicsAssert(vertexSource != 0, "Empty vertex pointer");
			for (const char* i = vertexSource; *i != '\0'; ++i, ++vLen);
			const char* vp1 = vertexSource + 1;
			GraphicsAssert(vLen != 0, "Empty vertex shader");

			int fLen = 0;
			GraphicsAssert(fragmentSource != 0, "Empty fragment pointer");
			for (const char* i = fragmentSource; *i != '\0'; ++i, ++fLen);
			GraphicsAssert(fLen != 0, "Empty fragment shader");

			result.program = wasmGraphics_CompileShader(vertexSource, vLen, fragmentSource, fLen);
			result.success = result.program != 0;
			GraphicsAssert(result.success, "Could not compile shader");

			return result;
		}
	}
}

/// Texture
void Graphics::Texture::SetPCM(bool pcm) {
	GLenum attachTarget = GL_TEXTURE_2D;

	GLenum compareMode = GL_NONE;
	GLenum compareFunc = GL_LEQUAL;
	if (pcm) {
		compareMode = GL_COMPARE_REF_TO_TEXTURE;
		compareFunc = GL_LEQUAL;
	}
	

	wasmGraphics_SetTexturePCM(mId, attachTarget, compareMode, compareFunc);
}

void Graphics::Texture::Set(void* data, TextureFormat dataFormat, u32 width, u32 height, bool genMipMaps) {
	GLenum internalFormat = Internal::TextureGetInternalFormatFromEnum(mInternalFormat);
	Internal::TextureFormatResult f = Internal::TextureGetDataFormatFromEnum(dataFormat);

	mWidth = width;
	mHeight = height;
	mIsMipMapped = genMipMaps;

	GraphicsAssert(mWidth > 0, "Width must be > 0");
	GraphicsAssert(mHeight > 0, "Height must be > 0");

	wasmGraphics_TextureSetData(mId, internalFormat,  width,  height, f.dataFormat, f.dataType, data, genMipMaps);
}

/// Device

void Graphics::Device::SetFaceVisibility(CullFace cull, FaceWind wind) {
	bool enableCullFace = false;
	bool disableCullFace = false;
	GLenum cullFaceType = GL_BACK;
	bool changeFace = false;
	GLenum faceWind = GL_CCW;

	if (mFaceCulling != cull) {
		if (cull == CullFace::Back) {
			if (mFaceCulling == CullFace::Off) {
				enableCullFace = true;
			}
			cullFaceType = GL_BACK;
		}
		else if (cull == CullFace::Front) {
			if (mFaceCulling == CullFace::Off) {
				enableCullFace = true;
			}
			cullFaceType = GL_FRONT;
		}
		else if (cull == CullFace::FrontAndBack) {
			if (mFaceCulling == CullFace::Off) {
				enableCullFace = true;
			}
			cullFaceType = GL_FRONT_AND_BACK;
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
			faceWind = GL_CCW;
		}
		else {
			faceWind = GL_CW;
		}
		mWindingOrder = wind;
	}

	if (enableCullFace || disableCullFace || changeFace) {
		wasmGraphics_DeviceSetFaceVisibility(enableCullFace, disableCullFace, cullFaceType, changeFace, faceWind);
	}
}

void Graphics::Device::SetDepthState(bool enable, DepthFunc depthFunc, f32* depthRange) {
	bool changeDepthState = false;
	GLenum depthState = GL_DEPTH_TEST;
	bool changeDepthFunc = false;
	GLenum func = GL_NONE;
	bool changeDepthRange = false;
	float depthRangeMin = 0.0f;
	float depthRangeMax = 0.0f;

	if (mDepth != enable) {
		changeDepthState = true;
		if (enable) {
			depthState = GL_DEPTH_TEST;
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
		wasmGraphics_SetDepthState(changeDepthState, depthState, changeDepthFunc,  func,  changeDepthRange,  depthRangeMin, depthRangeMax);
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
		updateRect = true;
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
	result->mReadBufferConfig = GL_FRONT;

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
	GraphicsAssert(compileStatus.success, "Failed to compile shader");

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
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,
	};

	int frameBufferId = 0;
	int numAttachments = 0;

	if (frameBuffer != 0) {
		frameBufferId = frameBuffer->mId;
		mBoundFrameBuffer = frameBuffer->mId;
		numAttachments = frameBuffer->TargetCount();
		if (numAttachments == 0) {
			frameBuffer->mReadBufferConfig = GL_NONE;
		}
		else {
			frameBuffer->mReadBufferConfig = GL_FRONT;
		}
		GraphicsAssert(numAttachments <= 8, "Only supports up to 8 color attachments");
	}
	else if (mBoundFrameBuffer != 0) {
		const u32 back_attach[1] = {GL_BACK};
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
		{  // Unbind any previously bound textures
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
	GraphicsAssert(slot.valid, "Setting invalid uniform");
	GraphicsAssert(slot.valid || (!slot.valid && slot.id != 0), "Something messed with slot");
	GraphicsAssert(slot.shader == mBoundProgram, "Binding index to wrong shader");
	wasmGraphics_DeviceSetUniform((int)type, slot.id, count, data);
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
		else {
			min = GL_NEAREST;
		}
	}

	if (sampler.mag == Filter::Nearest) {
		mag = GL_NEAREST;
	}

	GLenum wrapS = GL_REPEAT;
	GLenum wrapT = GL_REPEAT;

	if (sampler.wrapS == WrapMode::Clamp) {
		wrapS = GL_CLAMP_TO_EDGE;
	}

	if (sampler.wrapT == WrapMode::Clamp) {
		wrapT = GL_CLAMP_TO_EDGE;
	}

	GLenum target = GL_TEXTURE_2D;

	// Find texture unit
	u32 textureUnit = 33;
	u32 firstFree = 33;
	for (u32 i = 0; i < 32; ++i) {
		if (mBoundTextures[i].texture != 0) { // Something is bound
			if (mBoundTextures[i].index.valid && mBoundTextures[i].index.id == uniformSlot.id) { // Re-use
				textureUnit = i;
				GraphicsAssert(target == mBoundTextures[i].target, "Binding invalid texture types");
				break;
			}
		}
		else if (firstFree == 33) {
			GraphicsAssert(!mBoundTextures[i].index.valid, "free slot should not be valid");
			firstFree = i;
		}
	}
	if (textureUnit == 33) {
		textureUnit = firstFree;
		mBoundTextures[firstFree].index = uniformSlot;
		GraphicsAssert(mBoundTextures[firstFree].index.valid, "Found invalid index");
		mBoundTextures[firstFree].target = target;
		mBoundTextures[firstFree].texture = &texture;
	}
	GraphicsAssert(textureUnit < 33, "Invalid texture unit");

	
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

	wasmGraphics_DeviceBindTexture(enumTextureUnit, textureUnit, target, texture.mId, uniformSlot.id, min, mag, wrapS, wrapT, wrapT, updateSampler);
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
	Index result(0, false, mProgram);

	if (location >= 0) {
		result = Index(location, true, mProgram);
	}

	return result;
}

Graphics::Index Graphics::Shader::GetUniform(const char* name) {
	u32 wasmLen = 0;
	for (const char* i = name; name != 0 && *i != '\0'; ++i, ++wasmLen);

	int location = wasmGraphics_ShaderGetUniform(mProgram, name, wasmLen);
	Index result(0, false, mProgram);

	if (location >= 0) {
		result = Index(location, true, mProgram);
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

	GLenum attachTarget = GL_TEXTURE_2D;

	color.mCachedMin = GL_LINEAR;
	color.mCachedMag = GL_LINEAR;

	mColor[attachmentIndex] = &color;
	mOwner->mBoundFrameBuffer = 0;
	wasmGraphics_FramebufferAttachColor(attachTarget, mId, color.mId, attachmentIndex);
}

void Graphics::FrameBuffer::AttachDepth(Texture& depth, bool pcm) {
	GLenum attachTarget = GL_TEXTURE_2D;

	depth.mCachedMin = GL_LINEAR;
	depth.mCachedMag = GL_LINEAR;

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

	GLenum gl_filter = GL_NEAREST;
	if (filter == Filter::Linear) {
		gl_filter = GL_LINEAR;
	}

	mOwner->mBoundFrameBuffer = 0;
	wasmGraphics_FrameBufferResolveTo(mId, targetId, x0, y0, x1, y1, x2, y2, x3, y3, colorBit, depthBit, gl_filter);
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



#define export __attribute__ (( visibility( "default" ) )) extern "C"

export void wasmGraphics_Assert(bool cond, const char* loc, const char* msg) {
	Graphics::Internal::Assert(cond, loc, msg);
}

export Graphics::Device* wasm_Graphics_Initialize(Graphics::fpRequest allocPtr, Graphics::fpRelease releasePtr) {
	Graphics::Dependencies platform;
	platform.Request = allocPtr;
	platform.Release = releasePtr;
	Graphics::Device* device = (Graphics::Device*)allocPtr(sizeof(Graphics::Device));
	return Graphics::Initialize(*device, platform);
}

export void wasm_Graphics_Shutdown(Graphics::Device* device) {
	Graphics::Shutdown(*device);
}