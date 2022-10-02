'use strict';
/*jshint esversion: 6 */
/*jshint bitwise: false*/

var GlobalGraphicsManager = null;

class GraphicsManager {
    // https://webglreport.com/?v=2
    constructor(gl) {
        this.gl = gl;   
        this.env = null;
        this.devicePtr = null;
        this.exports = null;
        this.memory = null;
        this.memory_u8 = null;
        this.decoder =  new TextDecoder();

        this.textures = [];
        this.textures.push(null);

        this.vaos = [];
        this.vaos.push(null);

        this.fbos = [];
        this.fbos.push(null);

        this.vbos = [];
        this.vbos.push(null); 

        this.shaders = [];
        this.shaders.push(null);

        this.currentlyBoundShader = null;

        GlobalGraphicsManager = this;

        this.CheckError = function() { // TODO: Remove calls to CheckError from in this file. Add to main loop.
            let glError = GlobalGraphicsManager.gl.getError();
            if (glError != GlobalGraphicsManager.gl.NO_ERROR) {
                console.error("gl Error");
            }
            return glError;
        }
    }

    InjectWebAssemblyImportObject(wasmImportObject) {
        GlobalGraphicsManager.env = wasmImportObject.env;
        wasmImportObject.graphicsManager = GlobalGraphicsManager;
        
        GlobalGraphicsManager.env.wasmGraphics_Log = GlobalGraphicsManager.wasmGraphics_Log;
        GlobalGraphicsManager.env.wasmGraphics_SetTexturePCM = GlobalGraphicsManager.wasmGraphics_SetTexturePCM;
        GlobalGraphicsManager.env.wasmGraphics_TextureSetData = GlobalGraphicsManager.wasmGraphics_TextureSetData;
        GlobalGraphicsManager.env.wasmGraphics_DeviceSetFaceVisibility = GlobalGraphicsManager.wasmGraphics_DeviceSetFaceVisibility;
        GlobalGraphicsManager.env.wasmGraphics_DeviceClearRGBAD = GlobalGraphicsManager.wasmGraphics_DeviceClearRGBAD;
        GlobalGraphicsManager.env.wasmGraphics_SetDepthState = GlobalGraphicsManager.wasmGraphics_SetDepthState;
        GlobalGraphicsManager.env.wasmGraphics_DeviceClearBufferBits = GlobalGraphicsManager.wasmGraphics_DeviceClearBufferBits;
        GlobalGraphicsManager.env.wasmGraphics_DeviceWriteMask = GlobalGraphicsManager.wasmGraphics_DeviceWriteMask;
        GlobalGraphicsManager.env.wasmGraphics_SetGLBlendFuncEnabled = GlobalGraphicsManager.wasmGraphics_SetGLBlendFuncEnabled;
        GlobalGraphicsManager.env.wasmGraphics_UpdateGLBlendColor = GlobalGraphicsManager.wasmGraphics_UpdateGLBlendColor;
        GlobalGraphicsManager.env.wasmGraphics_ChangeGLBlendFuncSame = GlobalGraphicsManager.wasmGraphics_ChangeGLBlendFuncSame;
        GlobalGraphicsManager.env.wasmGraphics_ChangeGLBlendFuncSeperate = GlobalGraphicsManager.wasmGraphics_ChangeGLBlendFuncSeperate;
        GlobalGraphicsManager.env.wasmGraphics_ChangeGLBlendEquation = GlobalGraphicsManager.wasmGraphics_ChangeGLBlendEquation;
        GlobalGraphicsManager.env.wasmGraphics_ChangeGLBlendEquationSeparate = GlobalGraphicsManager.wasmGraphics_ChangeGLBlendEquationSeparate;
        GlobalGraphicsManager.env.wasmGraphics_DeviceClearColor = GlobalGraphicsManager.wasmGraphics_DeviceClearColor;
        GlobalGraphicsManager.env.wasmGraphics_DeviceClearDepth = GlobalGraphicsManager.wasmGraphics_DeviceClearDepth;
        GlobalGraphicsManager.env.wasmGraphics_SetGLViewport = GlobalGraphicsManager.wasmGraphics_SetGLViewport;
        GlobalGraphicsManager.env.wasmGraphics_DeviceSetScissorState = GlobalGraphicsManager.wasmGraphics_DeviceSetScissorState;
        GlobalGraphicsManager.env.wasmGraphics_GLGenFrameBuffer = GlobalGraphicsManager.wasmGraphics_GLGenFrameBuffer;
        GlobalGraphicsManager.env.wasmGraphics_DestroyFrameBuffer = GlobalGraphicsManager.wasmGraphics_DestroyFrameBuffer;
        GlobalGraphicsManager.env.wasmGraphics_GLGenBuffer = GlobalGraphicsManager.wasmGraphics_GLGenBuffer;
        GlobalGraphicsManager.env.wasmGraphics_GLDestroyBuffer = GlobalGraphicsManager.wasmGraphics_GLDestroyBuffer;
        GlobalGraphicsManager.env.wasmGraphics_GLCreateVAO = GlobalGraphicsManager.wasmGraphics_GLCreateVAO;
        GlobalGraphicsManager.env.wasmGraphics_GLDestroyVAO = GlobalGraphicsManager.wasmGraphics_GLDestroyVAO;
        GlobalGraphicsManager.env.wasmGraphics_CreateTexture = GlobalGraphicsManager.wasmGraphics_CreateTexture;
        GlobalGraphicsManager.env.wasmGraphics_GLDestroyTexture = GlobalGraphicsManager.wasmGraphics_GLDestroyTexture;
        GlobalGraphicsManager.env.wasmGraphics_GLDestroyShader = GlobalGraphicsManager.wasmGraphics_GLDestroyShader;
        GlobalGraphicsManager.env.wasmGraphics_DeviceSetRenderTarget = GlobalGraphicsManager.wasmGraphics_DeviceSetRenderTarget;
        GlobalGraphicsManager.env.wasmGraphics_DeviceDraw = GlobalGraphicsManager.wasmGraphics_DeviceDraw;
        GlobalGraphicsManager.env.wasmGraphics_VertexLayoutSet = GlobalGraphicsManager.wasmGraphics_VertexLayoutSet;
        GlobalGraphicsManager.env.wasmGraphics_VertexLayoutSetIndexBuffer = GlobalGraphicsManager.wasmGraphics_VertexLayoutSetIndexBuffer;
        GlobalGraphicsManager.env.wasmGraphics_FramebufferAttachColor = GlobalGraphicsManager.wasmGraphics_FramebufferAttachColor;
        GlobalGraphicsManager.env.wasmGraphics_FrameBufferAttachDepth = GlobalGraphicsManager.wasmGraphics_FrameBufferAttachDepth;
        GlobalGraphicsManager.env.wasmGraphics_FrameBufferIsValid = GlobalGraphicsManager.wasmGraphics_FrameBufferIsValid;
        GlobalGraphicsManager.env.wasmGraphics_FrameBufferResolveTo = GlobalGraphicsManager.wasmGraphics_FrameBufferResolveTo;
        GlobalGraphicsManager.env.wasmGraphics_BufferReset = GlobalGraphicsManager.wasmGraphics_BufferReset;
        GlobalGraphicsManager.env.wasmGraphics_BufferSet = GlobalGraphicsManager.wasmGraphics_BufferSet;
        GlobalGraphicsManager.env.wasmGraphics_ShaderGetUniform = GlobalGraphicsManager.wasmGraphics_ShaderGetUniform;
        GlobalGraphicsManager.env.wasmGraphics_ShaderGetAttribute = GlobalGraphicsManager.wasmGraphics_ShaderGetAttribute;
        GlobalGraphicsManager.env.wasmGraphics_ResetVertexLayout = GlobalGraphicsManager.wasmGraphics_ResetVertexLayout;
        GlobalGraphicsManager.env.wasmGraphics_BindVAO = GlobalGraphicsManager.wasmGraphics_BindVAO;
        GlobalGraphicsManager.env.wasmGraphics_DeviceBindTexture = GlobalGraphicsManager.wasmGraphics_DeviceBindTexture;
        GlobalGraphicsManager.env.wasmGraphics_DeviceSetUniform = GlobalGraphicsManager.wasmGraphics_DeviceSetUniform;
        GlobalGraphicsManager.env.wasmGraphics_DeviceBindShader = GlobalGraphicsManager.wasmGraphics_DeviceBindShader;
        GlobalGraphicsManager.env.wasmGraphics_SetDefaultGLState = GlobalGraphicsManager.wasmGraphics_SetDefaultGLState;
        GlobalGraphicsManager.env.wasmGraphics_GetScissorAndViewport = GlobalGraphicsManager.wasmGraphics_GetScissorAndViewport;
        GlobalGraphicsManager.env.wasmGraphics_CompileShader = GlobalGraphicsManager.wasmGraphics_CompileShader;
    }

    InitializeWebAssemblyDevice(allocPtr, releasePtr, wasmExports, wasmMemory) {
        GlobalGraphicsManager.exports = wasmExports;
        GlobalGraphicsManager.memory = wasmMemory;
        GlobalGraphicsManager.memory_u8 = new Uint8Array(GlobalGraphicsManager.memory.buffer, 0, GlobalGraphicsManager.memory.buffer.byteLength);
        GlobalGraphicsManager.devicePtr = GlobalGraphicsManager.exports.wasm_Graphics_Initialize(allocPtr, releasePtr);
    }

    ShutdownWebAssemblyDevice() {
        GlobalGraphicsManager.exports.wasm_Graphics_Shutdown(GlobalGraphicsManager.devicePtr);
    }

    // Internal functions
    wasmGraphics_Log(ptr_loc, int_locLen, ptr_msg, int_msgLen) {
        let str_message = GlobalGraphicsManager.decoder.decode(new Uint8Array(GlobalGraphicsManager.memory.buffer, ptr_msg, int_msgLen));
        const str_location = GlobalGraphicsManager.decoder.decode(new Uint8Array(GlobalGraphicsManager.memory.buffer, ptr_loc, int_locLen));

        if (str_message.length == 0) {
            str_message = "<message empty>";
        }
        if (str_location == 0) {
            str_message = "<location unknown>";
        }

        console.log(str_message + "\n\t" + str_location);
    }

    wasmGraphics_CreateTexture() {
        let glTexture = GlobalGraphicsManager.gl.createTexture();

        let insertIndex = -1;
        for (let i = 1; i < GlobalGraphicsManager.textures.length; ++i) {
            if (GlobalGraphicsManager.textures[i] == null) {
                GlobalGraphicsManager.textures[i] = glTexture;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = GlobalGraphicsManager.textures.length;
            GlobalGraphicsManager.textures.push(glTexture);
        }

        // TODO: Remove these, only adding to investigate bad normals
        if (insertIndex == 2) {
            glTexture.debugName = "Shadowmap";
        }
        if (insertIndex == 3) {
            glTexture.debugName = "albedo";
        }
        if (insertIndex == 4) {
            glTexture.debugName = "Normal";
        }

        return insertIndex;
    }

    wasmGraphics_GLDestroyTexture(int_texId) {
        if (int_texId == 0) {
            console.log("Trying to delete invalid texture: 0");
        }
        GlobalGraphicsManager.gl.deleteTexture(GlobalGraphicsManager.textures[int_texId]);
        GlobalGraphicsManager.textures[int_texId] = null;
    }

    wasmGraphics_GLCreateVAO() {
        let glVao = GlobalGraphicsManager.gl.createVertexArray();

        let insertIndex = -1;
        for (let i = 1; i < GlobalGraphicsManager.vaos.length; ++i) {
            if (GlobalGraphicsManager.vaos[i] == null) {
                GlobalGraphicsManager.vaos[i] = glVao;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = GlobalGraphicsManager.vaos.length;
            GlobalGraphicsManager.vaos.push(glVao);
        }

        return insertIndex;
    }

    wasmGraphics_GLDestroyVAO(int_vaoID) {
        if (int_vaoID == 0) {
            console.log("Trying to delete invalid VAO: 0");
        }
        GlobalGraphicsManager.gl.deleteVertexArray(GlobalGraphicsManager.vaos[int_vaoID]);
        GlobalGraphicsManager.vaos[int_vaoID] = null;
    }

    wasmGraphics_ResetVertexLayout(int_id) {
        GlobalGraphicsManager.wasmGraphics_GLDestroyVAO(int_id);
        return GlobalGraphicsManager.wasmGraphics_GLCreateVAO();
    }

    wasmGraphics_BufferReset(int_bufferId) {
        GlobalGraphicsManager.wasmGraphics_GLDestroyBuffer(int_bufferId);
	    return GlobalGraphicsManager.wasmGraphics_GLGenBuffer();
    }

    wasmGraphics_GLGenFrameBuffer() {
        let glFbo = GlobalGraphicsManager.gl.createFramebuffer();

        let insertIndex = -1;
        for (let i = 1; i < GlobalGraphicsManager.fbos.length; ++i) {
            if (GlobalGraphicsManager.fbos[i] == null) {
                GlobalGraphicsManager.fbos[i] = glFbo;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = GlobalGraphicsManager.fbos.length;
            GlobalGraphicsManager.fbos.push(glFbo);
        }

        return insertIndex;
    }

    wasmGraphics_DestroyFrameBuffer(int_bufferId) {
        if (int_bufferId == 0) {
            console.log("Trying to delete invalid FBO: 0");
        }
        GlobalGraphicsManager.gl.deleteFramebuffer(GlobalGraphicsManager.fbos[int_bufferId]);
        GlobalGraphicsManager.fbos[int_bufferId] = null;
    }

    wasmGraphics_GLGenBuffer() {
        let glBuffer = GlobalGraphicsManager.gl.createBuffer();

        let insertIndex = -1;
        for (let i = 1; i < GlobalGraphicsManager.vbos.length; ++i) {
            if (GlobalGraphicsManager.vbos[i] == null) {
                GlobalGraphicsManager.vbos[i] = glBuffer;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = GlobalGraphicsManager.vbos.length;
            GlobalGraphicsManager.vbos.push(glBuffer);
        }

        return insertIndex;
    }

    wasmGraphics_GLDestroyBuffer(int_bufferId) {
        if (int_bufferId == 0) {
            console.log("Trying to delete invalid buffer object: 0");
        }
        GlobalGraphicsManager.gl.deleteBuffer(GlobalGraphicsManager.vbos[int_bufferId]);
        GlobalGraphicsManager.vbos[int_bufferId] = null;
    }

    wasmGraphics_SetTexturePCM(int_glTextureId, int_glTextureAttachTarget, int_glCompareMode, int_glCompareFunc) {
        let texture = GlobalGraphicsManager.textures[int_glTextureId];
        GlobalGraphicsManager.gl.bindTexture(int_glTextureAttachTarget, texture);
        GlobalGraphicsManager.gl.texParameteri(int_glTextureAttachTarget, GlobalGraphicsManager.gl.TEXTURE_COMPARE_MODE, int_glCompareMode);
		GlobalGraphicsManager.gl.texParameteri(int_glTextureAttachTarget, GlobalGraphicsManager.gl.TEXTURE_COMPARE_FUNC, int_glCompareFunc);
        GlobalGraphicsManager.gl.bindTexture(int_glTextureAttachTarget, null);
    }

    wasmGraphics_TextureSetData(int_glTextureId, int_glInternalFormat, int_width, int_height, int_glDataFormat, int_glDataFormatType, ptr_data, bool_genMipMaps) {
        if (int_width == 0 || int_height == 0) {
            console.log("Bad size");
        }

        let texture = GlobalGraphicsManager.textures[int_glTextureId];
        
        GlobalGraphicsManager.gl.bindTexture(GlobalGraphicsManager.gl.TEXTURE_2D, texture); 
        let isDepth = int_glInternalFormat == GlobalGraphicsManager.gl.DEPTH_COMPONENT24 ||
                        int_glInternalFormat == GlobalGraphicsManager.gl.DEPTH_COMPONENT16 ||
                        int_glInternalFormat == GlobalGraphicsManager.gl.DEPTH_COMPONENT32F ||
                        int_glInternalFormat == GlobalGraphicsManager.gl.DEPTH32F_STENCIL8 ||
                        int_glInternalFormat == GlobalGraphicsManager.gl.DEPTH_COMPONENT;
        if (isDepth) {
            GlobalGraphicsManager.gl.texImage2D(GlobalGraphicsManager.gl.TEXTURE_2D, 0, int_glInternalFormat, int_width, int_height, 0, GlobalGraphicsManager.gl.DEPTH_COMPONENT, int_glDataFormatType, null);
            //GlobalGraphicsManager.gl.texStorage2D(GlobalGraphicsManager.gl.TEXTURE_2D, 0, int_glInternalFormat, int_width, int_height);
        }
        else {
            if (ptr_data == 0) {
                ptr_data = null;
            }
            GlobalGraphicsManager.gl.texImage2D(GlobalGraphicsManager.gl.TEXTURE_2D, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataFormatType, GlobalGraphicsManager.memory_u8, ptr_data);
        }

        if (bool_genMipMaps) {
            GlobalGraphicsManager.gl.generateMipmap(GlobalGraphicsManager.gl.TEXTURE_2D);
        }
        GlobalGraphicsManager.gl.bindTexture(GlobalGraphicsManager.gl.TEXTURE_2D, null);
    }

    wasmGraphics_DeviceSetFaceVisibility(bool_enableCullFace, bool_disableCullFace, int_cullFaceType, bool_changeFace, int_faceWind) {
        if (bool_enableCullFace) {
            GlobalGraphicsManager.gl.enable(GlobalGraphicsManager.gl.CULL_FACE);
            GlobalGraphicsManager.gl.cullFace(int_cullFaceType);
        }
        else if (bool_disableCullFace) {
            GlobalGraphicsManager.gl.disable(GlobalGraphicsManager.gl.CULL_FACE);
        }

        if (bool_changeFace) {
            GlobalGraphicsManager.gl.frontFace(int_faceWind);
        }
    }

    wasmGraphics_DeviceClearRGBAD(float_r, float_g, float_b, float_d) {
        GlobalGraphicsManager.gl.clearColor(float_r, float_g, float_b, 1.0);
        GlobalGraphicsManager.gl.clearDepth(float_d);
        GlobalGraphicsManager.gl.clear(GlobalGraphicsManager.gl.COLOR_BUFFER_BIT | GlobalGraphicsManager.gl.DEPTH_BUFFER_BIT);
    }

    wasmGraphics_SetDepthState(bool_changeDepthState, int_depthState, bool_changeDepthFunc, int_func, bool_changeDepthRange, float_depthRangeMin, float_depthRangeMax) {
        if (bool_changeDepthState) {
            if (int_depthState !== 0) {
                GlobalGraphicsManager.gl.enable(GlobalGraphicsManager.gl.DEPTH_TEST);
            }
            else {
                GlobalGraphicsManager.gl.disable(GlobalGraphicsManager.gl.DEPTH_TEST);
            }
        }
    
        if (bool_changeDepthFunc) {
            GlobalGraphicsManager.gl.depthFunc(int_func);
        }
    
        if (bool_changeDepthRange) {
            GlobalGraphicsManager.gl.depthRange(float_depthRangeMin, float_depthRangeMax);
        }
    }

    wasmGraphics_DeviceClearBufferBits(bool_color, bool_depth) {
        if (bool_color && !bool_depth) {
            GlobalGraphicsManager.gl.clear(GlobalGraphicsManager.gl.COLOR_BUFFER_BIT);
        }
        else if (!bool_color && bool_depth) {
            GlobalGraphicsManager.gl.clear(GlobalGraphicsManager.gl.DEPTH_BUFFER_BIT);
        }
        else if (bool_color && bool_depth) {
            GlobalGraphicsManager.gl.clear(GlobalGraphicsManager.gl.COLOR_BUFFER_BIT | GlobalGraphicsManager.gl.DEPTH_BUFFER_BIT);
        }
    }

    wasmGraphics_DeviceWriteMask(bool_r, bool_g, bool_b, bool_a, bool_depth) {
        GlobalGraphicsManager.gl.colorMask(bool_r, bool_g, bool_b, bool_a);
	    GlobalGraphicsManager.gl.depthMask(bool_depth);
    }

    wasmGraphics_SetGLBlendFuncEnabled(bool_state) {
        if (bool_state) {
            GlobalGraphicsManager.gl.enable(GlobalGraphicsManager.gl.BLEND);
        }
        else {
            GlobalGraphicsManager.gl.disable(GlobalGraphicsManager.gl.BLEND);
        }
    }

    wasmGraphics_UpdateGLBlendColor(float_r, float_g, float_b, float_a) {
		GlobalGraphicsManager.gl.blendColor(float_r, float_g, float_b, float_a);
    }

    wasmGraphics_ChangeGLBlendFuncSame(int_srcRgb, int_dstRgb) {
		GlobalGraphicsManager.gl.blendFunc(int_srcRgb, int_dstRgb);
    }

    wasmGraphics_ChangeGLBlendFuncSeperate(int_srcRgb, int_dstRgb, int_srcAlpha, int_dstAlpha) {
		GlobalGraphicsManager.gl.blendFuncSeparate(int_srcRgb, int_dstRgb, int_srcAlpha, int_dstAlpha);
    }

    wasmGraphics_ChangeGLBlendEquation(int_rgbEquation) {
        GlobalGraphicsManager.gl.blendEquation(int_rgbEquation);
    }

    wasmGraphics_ChangeGLBlendEquationSeparate(int_rgbEquation, int_alphaEquation) {
        GlobalGraphicsManager.gl.blendEquationSeparate(int_rgbEquation, int_alphaEquation);
    }

    wasmGraphics_DeviceClearColor(float_r, float_g, float_b) {
        GlobalGraphicsManager.gl.clearColor(float_r, float_g, float_b, 1.0);
	    GlobalGraphicsManager.gl.clear(GlobalGraphicsManager.gl.COLOR_BUFFER_BIT);
    }

    wasmGraphics_DeviceClearDepth(float_depth) {
        GlobalGraphicsManager.gl.clearDepth(float_depth);
	    GlobalGraphicsManager.gl.clear(GlobalGraphicsManager.gl.DEPTH_BUFFER_BIT);
    }

    wasmGraphics_SetGLViewport(int_x, int_y, int_w, int_h) {
        GlobalGraphicsManager.gl.viewport(int_x, int_y, int_w, int_h);
    }

    wasmGraphics_DeviceSetScissorState(bool_enableScissor, bool_disableScissor, bool_updateRect, int_x, int_y, int_w, int_h) {
        if (bool_enableScissor) {
            GlobalGraphicsManager.gl.enable(GlobalGraphicsManager.gl.SCISSOR_TEST);
        }
        else if (bool_disableScissor) {
            GlobalGraphicsManager.gl.disable(GlobalGraphicsManager.gl.SCISSOR_TEST);
        }

        if (bool_updateRect) {
		    GlobalGraphicsManager.gl.scissor(int_x, int_y, int_w, int_h);
        }
    }

    wasmGraphics_DeviceSetRenderTarget(int_frameBufferId, int_numAttachments) {
        if (int_frameBufferId != 0) {
            let fbo = GlobalGraphicsManager.fbos[int_frameBufferId];
            GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, fbo);

            if (int_numAttachments == 0) {
                GlobalGraphicsManager.gl.drawBuffers([GlobalGraphicsManager.gl.NONE]);
                GlobalGraphicsManager.gl.readBuffer(GlobalGraphicsManager.gl.NONE);
            }
            else {
                let buffers = [
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT0, 
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT1,
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT2,
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT3,
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT4,
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT5,
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT6,
                    GlobalGraphicsManager.gl.COLOR_ATTACHMENT7,
                ];
                while(buffers.length > int_numAttachments && buffers.length != 0) {
                    buffers.pop();
                }

                GlobalGraphicsManager.gl.drawBuffers(buffers);
                GlobalGraphicsManager.gl.readBuffer(GlobalGraphicsManager.gl.FRONT);
            }
            
        }
        else {
            let buffers = [GlobalGraphicsManager.gl.BACK];
            GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, null);
            GlobalGraphicsManager.gl.drawBuffers(buffers);
        }
    }

    wasmGraphics_DeviceDraw(int_glVao, bool_indexed, int_instanceCount, int_drawMode, int_startIndex, int_indexCount, int_bufferType) {
        let vao = GlobalGraphicsManager.vaos[int_glVao];
        GlobalGraphicsManager.gl.bindVertexArray(vao);
        if (bool_indexed) {
            if (int_instanceCount <= 1) {
                GlobalGraphicsManager.gl.drawElements(int_drawMode, int_indexCount, int_bufferType, int_startIndex);
            }
            else {
                GlobalGraphicsManager.gl.drawElementsInstanced(int_drawMode, int_indexCount, int_bufferType, int_startIndex, int_instanceCount);
            }
        }
        else {
            if (int_instanceCount <= 1) {
                GlobalGraphicsManager.gl.drawArrays(int_drawMode, int_startIndex, int_indexCount);
            }
            else {
                GlobalGraphicsManager.gl.drawArraysInstanced(int_drawMode, int_startIndex, int_indexCount, int_instanceCount);
            }
        }
    }

    wasmGraphics_VertexLayoutSet(int_glVaoId, int_glBufferId, int_slotId, int_numComponents, int_type, int_stride, int_offset, int_divisor) {
        let vao = GlobalGraphicsManager.vaos[int_glVaoId];
        let buffer = GlobalGraphicsManager.vbos[int_glBufferId];

        GlobalGraphicsManager.gl.bindVertexArray(vao);
        GlobalGraphicsManager.gl.bindBuffer(GlobalGraphicsManager.gl.ARRAY_BUFFER, buffer);
        GlobalGraphicsManager.gl.vertexAttribPointer(int_slotId, int_numComponents, int_type, false, int_stride, int_offset);
        GlobalGraphicsManager.gl.enableVertexAttribArray(int_slotId);
        GlobalGraphicsManager.gl.vertexAttribDivisor(int_slotId, int_divisor);
        GlobalGraphicsManager.gl.bindVertexArray(null);
    }

    wasmGraphics_VertexLayoutSetIndexBuffer(int_glVaoId, int_glElementArrayBufferId) {
        let vao = GlobalGraphicsManager.vaos[int_glVaoId];
        let buffer = GlobalGraphicsManager.vbos[int_glElementArrayBufferId];
        
        GlobalGraphicsManager.gl.bindVertexArray(vao);
        GlobalGraphicsManager.gl.bindBuffer(GlobalGraphicsManager.gl.ELEMENT_ARRAY_BUFFER, buffer);
        GlobalGraphicsManager.gl.bindVertexArray(null);
    }

    wasmGraphics_FramebufferAttachColor(int_attachTarget, int_frameBufferId, int_textureId, int_attachmentIndex) {
        let fbo = GlobalGraphicsManager.fbos[int_frameBufferId];
        let texture = GlobalGraphicsManager.textures[int_textureId];

	    GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, fbo);
	    GlobalGraphicsManager.gl.bindTexture(int_attachTarget, texture);
		GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_MIN_FILTER, GlobalGraphicsManager.gl.LINEAR);
		GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_MAG_FILTER, GlobalGraphicsManager.gl.LINEAR);
	    GlobalGraphicsManager.gl.framebufferTexture2D(GlobalGraphicsManager.gl.FRAMEBUFFER, GlobalGraphicsManager.gl.COLOR_ATTACHMENT0 + int_attachmentIndex, int_attachTarget, texture, 0);
	    GlobalGraphicsManager.gl.bindTexture(int_attachTarget, null);
	    GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, null);
    }

    wasmGraphics_FrameBufferAttachDepth(int_attachTarget, int_frameBufferId, int_textureId, bool_pcm) {
        let fbo = GlobalGraphicsManager.fbos[int_frameBufferId];
        let texture = GlobalGraphicsManager.textures[int_textureId];

        GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, fbo);
	    GlobalGraphicsManager.gl.bindTexture(int_attachTarget, texture);
        if (bool_pcm) {
            GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_COMPARE_MODE, GlobalGraphicsManager.gl.COMPARE_REF_TO_TEXTURE);
            GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_COMPARE_FUNC, GlobalGraphicsManager.gl.LEQUAL);
        }
        else if (bool_pcm) {
            GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_COMPARE_MODE, GlobalGraphicsManager.gl.NONE);
            GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_COMPARE_FUNC, GlobalGraphicsManager.gl.LEQUAL);
        }

        GlobalGraphicsManager.CheckError();
        GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_MIN_FILTER, GlobalGraphicsManager.gl.LINEAR);
		GlobalGraphicsManager.gl.texParameteri(int_attachTarget, GlobalGraphicsManager.gl.TEXTURE_MAG_FILTER, GlobalGraphicsManager.gl.LINEAR);
	    GlobalGraphicsManager.gl.framebufferTexture2D(GlobalGraphicsManager.gl.FRAMEBUFFER, GlobalGraphicsManager.gl.DEPTH_ATTACHMENT, int_attachTarget, texture, 0);
        GlobalGraphicsManager.gl.bindTexture(int_attachTarget, null);
	    GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, null);
    }

    wasmGraphics_FrameBufferIsValid(int_frameBufferId) {
        let fbo = GlobalGraphicsManager.fbos[int_frameBufferId];
        GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, fbo);
        let result = GlobalGraphicsManager.gl.checkFramebufferStatus(GlobalGraphicsManager.gl.FRAMEBUFFER) == GlobalGraphicsManager.gl.FRAMEBUFFER_COMPLETE;
        GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, null);
        return result;
    }

    wasmGraphics_FrameBufferResolveTo(int_readBuffer, int_drawBuffer, int_x0, int_y0, int_x1, int_y1, int_x2, int_y2, int_x3, int_y3, bool_colorBit, bool_depthBit, int_filter) {
        let read_fbo = GlobalGraphicsManager.fbos[int_readBuffer];
        let draw_fbo = GlobalGraphicsManager.fbos[int_drawBuffer];
       
        GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.READ_FRAMEBUFFER, read_fbo);
        GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.DRAW_FRAMEBUFFER, draw_fbo);
        let mask = 0;
        if (bool_depthBit) {
		    mask |= GlobalGraphicsManager.gl.DEPTH_BUFFER_BIT;
        }
        if (bool_colorBit) {
            mask |= GlobalGraphicsManager.gl.COLOR_BUFFER_BIT;
        }
        GlobalGraphicsManager.gl.blitFramebuffer(int_x0, int_y0, int_x1, int_y1, int_x2, int_y2, int_x3, int_y3, mask, int_filter);
        GlobalGraphicsManager.gl.bindFramebuffer(GlobalGraphicsManager.gl.FRAMEBUFFER, null);
    }

    wasmGraphics_BufferSet(bool_indexBuffer, int_bufferId, int_arraySizeInByfes, ptr_inputArray, bool_isStatic) {
        let buffer = GlobalGraphicsManager.vbos[int_bufferId];

        if (bool_indexBuffer) {
            GlobalGraphicsManager.gl.bindBuffer(GlobalGraphicsManager.gl.ELEMENT_ARRAY_BUFFER, buffer);
            GlobalGraphicsManager.gl.bufferData(GlobalGraphicsManager.gl.ELEMENT_ARRAY_BUFFER, GlobalGraphicsManager.memory_u8, GlobalGraphicsManager.gl.STATIC_DRAW, ptr_inputArray, int_arraySizeInByfes);
            GlobalGraphicsManager.gl.bindBuffer(GlobalGraphicsManager.gl.ELEMENT_ARRAY_BUFFER, null);
        }
        else {
            GlobalGraphicsManager.gl.bindBuffer(GlobalGraphicsManager.gl.ARRAY_BUFFER, buffer);
            GlobalGraphicsManager.gl.bufferData(GlobalGraphicsManager.gl.ARRAY_BUFFER, GlobalGraphicsManager.memory_u8, bool_isStatic? GlobalGraphicsManager.gl.STATIC_DRAW : GlobalGraphicsManager.gl.DYNAMIC_DRAW, ptr_inputArray, int_arraySizeInByfes);
            GlobalGraphicsManager.gl.bindBuffer(GlobalGraphicsManager.gl.ARRAY_BUFFER, null);
        }
    }

    wasmGraphics_ShaderGetUniform(int_program, ptr_name, int_name_len) {
        let program = GlobalGraphicsManager.shaders[int_program];
        const _array = new Uint8Array(GlobalGraphicsManager.memory.buffer, ptr_name, int_name_len);
        let name = GlobalGraphicsManager.decoder.decode(_array);

        if (program.uniformIndexMap.hasOwnProperty(name)) {
            return program.uniformIndexMap[name];
        }

        let uniformLocation = GlobalGraphicsManager.gl.getUniformLocation(program, name);

        if (uniformLocation == null) {
            return -1;
        }

        program.uniformIndexObjects.push(uniformLocation);
        let index = program.uniformIndexObjects.length - 1;

        program.uniformIndexMap[name] = index;
        program.uniformIndexObjects[index].uniformName = name;
        
        return index;
    }

    wasmGraphics_ShaderGetAttribute(int_program, ptr_name, int_name_len) {
        let program = GlobalGraphicsManager.shaders[int_program];
        const _array = new Uint8Array(GlobalGraphicsManager.memory.buffer, ptr_name, int_name_len);
        let name = GlobalGraphicsManager.decoder.decode(_array);

        return GlobalGraphicsManager.gl.getAttribLocation(program, name);
    }

    wasmGraphics_BindVAO(int_vaoId) {
        let vao = GlobalGraphicsManager.vaos[int_vaoId];
        GlobalGraphicsManager.gl.bindVertexArray(vao);
    }

    wasmGraphics_DeviceBindTexture(int_textureUnit, int_textureUnitIndex, int_textureTarget, int_textureId, int_uniformSlot, int_minFilter, int_magFilter, int_wrapS, int_wrapT, int_wrapR, bool_updateSampler) {
	    let texture = GlobalGraphicsManager.textures[int_textureId];

        GlobalGraphicsManager.gl.activeTexture(int_textureUnit);
        GlobalGraphicsManager.gl.bindTexture(int_textureTarget, texture);

        if (bool_updateSampler) {
            GlobalGraphicsManager.gl.texParameteri(int_textureTarget, GlobalGraphicsManager.gl.TEXTURE_MIN_FILTER, int_minFilter);
            GlobalGraphicsManager.gl.texParameteri(int_textureTarget, GlobalGraphicsManager.gl.TEXTURE_MAG_FILTER, int_magFilter);
            GlobalGraphicsManager.gl.texParameteri(int_textureTarget, GlobalGraphicsManager.gl.TEXTURE_WRAP_S, int_wrapS);
            GlobalGraphicsManager.gl.texParameteri(int_textureTarget, GlobalGraphicsManager.gl.TEXTURE_WRAP_T, int_wrapT);
            GlobalGraphicsManager.gl.texParameteri(int_textureTarget, GlobalGraphicsManager.gl.TEXTURE_WRAP_R, int_wrapR);
        }

        let program = GlobalGraphicsManager.currentlyBoundShader;
        if (int_uniformSlot >= program.uniformIndexObjects.length) {
            console.log("Trying to access invalid texture");
        }
        let slot = program.uniformIndexObjects[int_uniformSlot];
	    GlobalGraphicsManager.gl.uniform1i(slot, int_textureUnitIndex);
    }

    wasmGraphics_DeviceSetUniform(int_type, int_slotId, int_count, ptr_data) {
        let program = GlobalGraphicsManager.currentlyBoundShader;
        let slot = program.uniformIndexObjects[int_slotId]; // TODO: The problem is slotId, it's always 0

        if (int_type == 0/* UniformType::Int1 */) {
            let intData = new Int32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 1);
            GlobalGraphicsManager.gl.Uniform1iv(slot, intData);
        }
        else if (int_type == 1/*UniformType::Int2*/) {
            let intData = new Int32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 2);
            GlobalGraphicsManager.gl.uniform2iv(slot, intData);
        }
        else if (int_type == 2/*UniformType::Int3*/) {
            let intData = new Int32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 3);
            GlobalGraphicsManager.gl.uniform3iv(slot, intData);
        }
        else if (int_type == 3/*UniformType::Int4*/) {
            let intData = new Int32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 4);
            GlobalGraphicsManager.gl.uniform4iv(slot, intData);
        }
        else if (int_type == 4/*UniformType::Float1*/) {
            let floatData = new Float32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 1);
            GlobalGraphicsManager.gl.uniform1fv(slot, floatData);
        }
        else if (int_type == 5/*UniformType::Float2*/) {
            let floatData = new Float32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 2);
            GlobalGraphicsManager.gl.uniform2fv(slot, floatData);
        }
        else if (int_type == 6/*UniformType::Float3*/) {
            let floatData = new Float32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count *3);
            GlobalGraphicsManager.gl.uniform3fv(slot, floatData);
        }
        else if (int_type == 7/*UniformType::Float4*/) {
            let floatData = new Float32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count *4);
            GlobalGraphicsManager.gl.uniform4fv(slot, floatData);
        }
        else if (int_type == 8/*UniformType::Float9*/) {
            let floatData = new Float32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 9);
            GlobalGraphicsManager.gl.uniformMatrix3fv(slot, false, floatData);
        }
        else if (int_type == 9/*UniformType::Float16*/) {
            let floatData = new Float32Array(GlobalGraphicsManager.memory.buffer, ptr_data, int_count * 16);
            GlobalGraphicsManager.gl.uniformMatrix4fv(slot, false, floatData);
        }
    }

    wasmGraphics_DeviceBindShader(int_programId, int_boundTextures) {
        let shader = GlobalGraphicsManager.shaders[int_programId];

		GlobalGraphicsManager.gl.useProgram(shader);
        GlobalGraphicsManager.currentlyBoundShader = shader;

        for (let i = 0; i < 32; ++i) {
            let set = int_boundTextures & (1 << i);
            if (set) {
                GlobalGraphicsManager.gl.activeTexture(GlobalGraphicsManager.gl.TEXTURE0 + i);
                GlobalGraphicsManager.gl.bindTexture(GlobalGraphicsManager.gl.TEXTURE_2D, null);
            }
        }
        GlobalGraphicsManager.gl.activeTexture(GlobalGraphicsManager.gl.TEXTURE0);
    }

    wasmGraphics_SetDefaultGLState() {
        GlobalGraphicsManager.gl.disable(GlobalGraphicsManager.gl.BLEND);
        GlobalGraphicsManager.gl.blendColor(0.0, 0.0, 0.0, 0.0);
        GlobalGraphicsManager.gl.blendFunc(GlobalGraphicsManager.gl.ONE, GlobalGraphicsManager.gl.ZERO);
        GlobalGraphicsManager.gl.blendEquation(GlobalGraphicsManager.gl.FUNC_ADD);
        GlobalGraphicsManager.gl.enable(GlobalGraphicsManager.gl.CULL_FACE);
        GlobalGraphicsManager.gl.cullFace(GlobalGraphicsManager.gl.BACK);
        GlobalGraphicsManager.gl.frontFace(GlobalGraphicsManager.gl.CCW);
        GlobalGraphicsManager.gl.enable(GlobalGraphicsManager.gl.DEPTH_TEST);
        GlobalGraphicsManager.gl.depthFunc(GlobalGraphicsManager.gl.LESS);
        GlobalGraphicsManager.gl.depthRange(0.0, 1.0);
        GlobalGraphicsManager.gl.disable(GlobalGraphicsManager.gl.SCISSOR_TEST);
        GlobalGraphicsManager.gl.scissor(0, 0, 800, 600);
    }

    wasmGraphics_GetScissorAndViewport(ptr_scissorPtr, ptr_viewPtr) {
        let scissorRect = GlobalGraphicsManager.gl.getParameter(GlobalGraphicsManager.gl.SCISSOR_BOX);
        let viewportRect = GlobalGraphicsManager.gl.getParameter(GlobalGraphicsManager.gl.VIEWPORT);

        let targetScissor = new Uint32Array(GlobalGraphicsManager.memory.buffer, ptr_scissorPtr, 4);
        let targetView =new Uint32Array(GlobalGraphicsManager.memory.buffer, ptr_viewPtr, 4);

        targetScissor[0] = scissorRect[0];
        targetScissor[1] = scissorRect[1];
        targetScissor[2] = scissorRect[2];
        targetScissor[3] = scissorRect[3];

        targetView[0] = viewportRect[0];
        targetView[1] = viewportRect[1];
        targetView[2] = viewportRect[2];
        targetView[3] = viewportRect[3];
    }

    wasmGraphics_CompileShader(ptr_vShader, int_vShaderLen, ptr_fShader, int_fShaderLen) {
        const vertexShaderStr = GlobalGraphicsManager.decoder.decode(new Uint8Array(GlobalGraphicsManager.memory.buffer, ptr_vShader, int_vShaderLen));
        const fragmentShaderStr = GlobalGraphicsManager.decoder.decode(new Uint8Array(GlobalGraphicsManager.memory.buffer, ptr_fShader, int_fShaderLen));
        
        let vertexShader = GlobalGraphicsManager.gl.createShader(GlobalGraphicsManager.gl.VERTEX_SHADER);
        GlobalGraphicsManager.gl.shaderSource(vertexShader, vertexShaderStr);
        GlobalGraphicsManager.gl.compileShader(vertexShader);
        let success = GlobalGraphicsManager.gl.getShaderParameter(vertexShader, GlobalGraphicsManager.gl.COMPILE_STATUS);
        if (!success) {
            let message = GlobalGraphicsManager.gl.getShaderInfoLog(vertexShader);
            console.log("Error compiling vertex shader: " + message);
            return 0;
        }

        let fragmentShader = GlobalGraphicsManager.gl.createShader(GlobalGraphicsManager.gl.FRAGMENT_SHADER);
        GlobalGraphicsManager.gl.shaderSource(fragmentShader, fragmentShaderStr);
        GlobalGraphicsManager.gl.compileShader(fragmentShader);
        success = GlobalGraphicsManager.gl.getShaderParameter(fragmentShader, GlobalGraphicsManager.gl.COMPILE_STATUS);
        if (!success) {
            let message = GlobalGraphicsManager.gl.getShaderInfoLog(fragmentShader);
            console.log("Error compiling fragment shader: " + message);
            return 0;
        }

        let shaderProgram = GlobalGraphicsManager.gl.createProgram();
        GlobalGraphicsManager.gl.attachShader(shaderProgram, vertexShader);
        GlobalGraphicsManager.gl.attachShader(shaderProgram, fragmentShader);
        GlobalGraphicsManager.gl.linkProgram(shaderProgram);
        success = GlobalGraphicsManager.gl.getProgramParameter(shaderProgram, GlobalGraphicsManager.gl.LINK_STATUS);
        if (!success) {
            let message = GlobalGraphicsManager.gl.getProgramInfoLog(shaderProgram);
            console.log("Error linking shader: " + message);
            return 0;
        }

        // Delete shaders
        GlobalGraphicsManager.gl.deleteShader(vertexShader);
        GlobalGraphicsManager.gl.deleteShader(fragmentShader);
        
        // Now put that shader into an array and return its index
        let insertIndex = -1;
        for (let i = 1; i < GlobalGraphicsManager.shaders.length; ++i) {
            if (GlobalGraphicsManager.shaders[i] == null) {
                GlobalGraphicsManager.shaders[i] = shaderProgram;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = GlobalGraphicsManager.shaders.length;
            GlobalGraphicsManager.shaders.push(shaderProgram);
        }

        // Uniforms are WebGL objects:(
        shaderProgram.uniformIndexMap = {};
        shaderProgram.uniformIndexObjects = [];
        // Attributes are just GLuint's

        shaderProgram.vertexShaderStr =vertexShaderStr;
        shaderProgram.fragmentShaderStr = fragmentShaderStr;

        return insertIndex;
    }

    wasmGraphics_GLDestroyShader(int_shaderId) {
        if (int_shaderId == 0) {
            console.log("Trying to delete invalid shader: 0");
        }
        GlobalGraphicsManager.gl.deleteProgram(GlobalGraphicsManager.shaders[int_shaderId]);
        GlobalGraphicsManager.shaders[int_shaderId] = null;
    }
}