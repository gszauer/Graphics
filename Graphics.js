'use strict';
/*jshint_esversion: 6 */
/*jshint_bitwise: false*/

// https://webglreport.com/?v=2

class GraphicsManager {
    // this.env -> Web assembly import object .env
    // this.gl -> WebGL object
    // this.devicePtr -> C++ device pointer created on injection
    // this.exports -> wasm exports
    // this.log -> log function or null
    // this.memory = wasm memory

    constructor(gl, logFunc) {
        this.gl = gl;
        this.log = logFunc;
        this.env = null;
        this.devicePtr = null;
        this.exports = null;
        this.memory = null;
        this.memory_u8 = null;
        this.decoder =  new TextDecoder();

        this.textures = [];
        this.textures.push(null); // Index 0 is invalid

        this.vaos = [];
        this.vaos.push(null); // Index 0 is invalid

        this.fbos = [];
        this.fbos.push(null); // Index 0 is invalid

        this.vbos = [];
        this.vbos.push(null); // Index 0 is invalid

        this.shaders = [];
        this.shaders.push(null);
    }

    InjectWebAssemblyImportObject(wasmImportObject) {
        this.env = wasmImportObject.env;
        wasmImportObject.graphicsManager = this;
        
        this.env["wasmGraphics_Log"] = this.wasmGraphics_Log;
        this.env["wasmGraphics_SetTexturePCM"] = this.wasmGraphics_SetTexturePCM;
        this.env["wasmGraphics_TextureSetData"] = this.wasmGraphics_TextureSetData;
        this.env["wasmGraphics_TextureSetCubemap"] = this.wasmGraphics_TextureSetCubemap;
        this.env["wasmGraphics_DeviceSetFaceVisibility"] = this.wasmGraphics_DeviceSetFaceVisibility;
        this.env["wasmGraphics_DeviceClearRGBAD"] = this.wasmGraphics_DeviceClearRGBAD;
        this.env["wasmGraphics_SetDepthState"] = this.wasmGraphics_SetDepthState;
        this.env["wasmGraphics_DeviceClearBufferBits"] = this.wasmGraphics_DeviceClearBufferBits;
        this.env["wasmGraphics_DeviceWriteMask"] = this.wasmGraphics_DeviceWriteMask;
        this.env["wasmGraphics_SetGLBlendFuncEnabled"] = this.wasmGraphics_SetGLBlendFuncEnabled;
        this.env["wasmGraphics_UpdateGLBlendColor"] = this.wasmGraphics_UpdateGLBlendColor;
        this.env["wasmGraphics_ChangeGLBlendFuncSame"] = this.wasmGraphics_ChangeGLBlendFuncSame;
        this.env["wasmGraphics_ChangeGLBlendFuncSeperate"] = this.wasmGraphics_ChangeGLBlendFuncSeperate;
        this.env["wasmGraphics_ChangeGLBlendEquation"] = this.wasmGraphics_ChangeGLBlendEquation;
        this.env["wasmGraphics_ChangeGLBlendEquationSeparate"] = this.wasmGraphics_ChangeGLBlendEquationSeparate;
        this.env["wasmGraphics_DeviceClearColor"] = this.wasmGraphics_DeviceClearColor;
        this.env["wasmGraphics_DeviceClearDepth"] = this.wasmGraphics_DeviceClearDepth;
        this.env["wasmGraphics_SetGLViewport"] = this.wasmGraphics_SetGLViewport;
        this.env["wasmGraphics_DeviceSetScissorState"] = this.wasmGraphics_DeviceSetScissorState;
        this.env["wasmGraphics_GLGenFrameBuffer"] = this.wasmGraphics_GLGenFrameBuffer;
        this.env["wasmGraphics_DestroyFrameBuffer"] = this.wasmGraphics_DestroyFrameBuffer;
        this.env["wasmGraphics_GLGenBuffer"] = this.wasmGraphics_GLGenBuffer;
        this.env["wasmGraphics_GLDestroyBuffer"] = this.wasmGraphics_GLDestroyBuffer;
        this.env["wasmGraphics_GLCreateVAO"] = this.wasmGraphics_GLCreateVAO;
        this.env["wasmGraphics_GLDestroyVAO"] = this.wasmGraphics_GLDestroyVAO;
        this.env["wasmGraphics_CreateTexture"] = this.wasmGraphics_CreateTexture;
        this.env["wasmGraphics_GLDestroyTexture"] = this.wasmGraphics_GLDestroyTexture;
        this.env["wasmGraphics_GLDestroyShader"] = this.wasmGraphics_GLDestroyShader;
        this.env["wasmGraphics_DeviceSetRenderTarget"] = this.wasmGraphics_DeviceSetRenderTarget;
        this.env["wasmGraphics_DeviceDraw"] = this.wasmGraphics_DeviceDraw;
        this.env["wasmGraphics_VertexLayoutSet"] = this.wasmGraphics_VertexLayoutSet;
        this.env["wasmGraphics_VertexLayoutSetIndexBuffer"] = this.wasmGraphics_VertexLayoutSetIndexBuffer;
        this.env["wasmGraphics_FramebufferAttachColor"] = this.wasmGraphics_FramebufferAttachColor;
        this.env["wasmGraphics_FrameBufferAttachDepth"] = this.wasmGraphics_FrameBufferAttachDepth;
        this.env["wasmGraphics_FrameBufferIsValid"] = this.wasmGraphics_FrameBufferIsValid;
        this.env["wasmGraphics_FrameBufferResolveTo"] = this.wasmGraphics_FrameBufferResolveTo;
        this.env["wasmGraphics_BufferReset"] = this.wasmGraphics_BufferReset;
        this.env["wasmGraphics_BufferSet"] = this.wasmGraphics_BufferSet;
        this.env["wasmGraphics_ShaderGetUniform"] = this.wasmGraphics_ShaderGetUniform;
        this.env["wasmGraphics_ShaderGetAttribute"] = this.wasmGraphics_ShaderGetAttribute;
        this.env["wasmGraphics_ResetVertexLayout"] = this.wasmGraphics_ResetVertexLayout;
        this.env["wasmGraphics_BindVAO"] = this.wasmGraphics_BindVAO;
        this.env["wasmGraphics_DeviceBindTexture"] = this.wasmGraphics_DeviceBindTexture;
        this.env["wasmGraphics_DeviceSetUniform"] = this.wasmGraphics_DeviceSetUniform;
        this.env["wasmGraphics_DeviceBindShader"] = this.wasmGraphics_DeviceBindShader;
        this.env["wasmGraphics_SetDefaultGLState"] = this.wasmGraphics_SetDefaultGLState;
        this.env["wasmGraphics_GetScissorAndViewport"] = this.wasmGraphics_GetScissorAndViewport;
        this.env["wasmGraphics_CompileShader"] = this.wasmGraphics_CompileShader;
    }

    InitializeWebAssemblyDevice(allocPtr, releasePtr, wasmExports, wasmMemory) {
        this.exports = wasmExports;
        this.memory = wasmMemory;
        this.memory_u8 = new Uint8Array(this.memory.buffer, 0, this.memory.buffer.byteLength);
        this.devicePtr = this.exports.wasm_Graphics_Initialize(allocPtr, releasePtr);
    }

    ShutdownWebAssemblyDevice() {
        this.exports.wasm_Graphics_Shutdown(this.devicePtr);
    }

    wasmGraphics_Log(ptr_loc, int_locLen, ptr_msg, int_msgLen) {
        if (this.log !== undefined && this.log !== null) {
            const str_message = this.decoder.decode(new Uint8Array(this.memory.buffer, ptr_msg, int_msgLen));
            const str_location = this.decoder.decode(new Uint8Array(this.memory.buffer, ptr_loc, int_locLen));

            this.log(str_message + "\n\t" + str_location);
        }
    }

    wasmGraphics_CreateTexture() {
        let glTexture = this.gl.createTexture();

        let insertIndex = -1;
        for (let i = 1; i < this.textures.length; ++i) {
            if (this.textures[i] == null) {
                this.textures[i] = glTexture;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = this.textures.length;
            this.textures.push(glTexture);
        }

        return insertIndex;
    }

    wasmGraphics_GLDestroyTexture(int_texId) {
        if (int_texId == 0) {
            this.log("Trying to delete invalid texture: 0");
        }
        this.gl.deleteTexture(this.textures[int_texId]);
        this.textures[int_texId] = null;
    }

    wasmGraphics_GLCreateVAO() {
        let glVao = this.gl.createVertexArray();

        let insertIndex = -1;
        for (let i = 1; i < this.vaos.length; ++i) {
            if (this.vaos[i] == null) {
                this.vaos[i] = glVao;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = this.vaos.length;
            this.vaos.push(glVao);
        }

        return insertIndex;
    }

    wasmGraphics_GLDestroyVAO(int_vaoID) {
        if (int_vaoID == 0) {
            this.log("Trying to delete invalid VAO: 0");
        }
        this.gl.deleteVertexArray(this.vaos[int_vaoID]);
        this.vaos[int_vaoID] = null;
    }

    wasmGraphics_GLGenFrameBuffer() {
        let glFbo = this.gl.createFramebuffer();

        let insertIndex = -1;
        for (let i = 1; i < this.fbos.length; ++i) {
            if (this.fbos[i] == null) {
                this.fbos[i] = glFbo;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = this.fbos.length;
            this.fbos.push(glFbo);
        }

        return insertIndex;
    }

    wasmGraphics_DestroyFrameBuffer(int_bufferId) {
        if (int_bufferId == 0) {
            this.log("Trying to delete invalid FBO: 0");
        }
        this.gl.deleteFramebuffer(this.fbos[int_bufferId]);
        this.fbos[int_bufferId] = null;
    }

    wasmGraphics_GLGenBuffer() {
        let glBuffer = this.gl.createBuffer();

        let insertIndex = -1;
        for (let i = 1; i < this.vbos.length; ++i) {
            if (this.vbos[i] == null) {
                this.vbos[i] = glBuffer;
                insertIndex = i;
                break;
            }
        }

        if (insertIndex == -1) {
            insertIndex = this.vbos.length;
            this.vbos.push(glBuffer);
        }

        return insertIndex;
    }

    wasmGraphics_GLDestroyBuffer(int_bufferId) {
        if (int_bufferId == 0) {
            this.log("Trying to delete invalid buffer object: 0");
        }
        this.gl.deleteBuffer(this.vbos[int_bufferId]);
        this.vbos[int_bufferId] = null;
    }

    wasmGraphics_SetTexturePCM(int_glTextureId, int_glTextureAttachTarget, int_glCompareMode, int_glCompareFunc) {
        let texture = this.textures[int_glTextureId];
        this.gl.bindTexture(int_glTextureAttachTarget, texture);
        this.gl.texParameteri(int_glTextureAttachTarget, this.gl.TEXTURE_COMPARE_MODE, int_glCompareMode);
		this.gl.texParameteri(int_glTextureAttachTarget, this.gl.TEXTURE_COMPARE_FUNC, int_glCompareFunc);
        this.gl.bindTexture(int_glTextureAttachTarget, null);
    }

    wasmGraphics_TextureSetData(int_glTextureId, int_glInternalFormat, int_width, int_height, int_glDataFormat, int_glDataFormatType, ptr_data, bool_genMipMaps) {
        let texture = this.textures[int_glTextureId];
        
        this.gl.bindTexture(this.gl.TEXTURE_2D, texture); 
        this.gl.texImage2D(this.gl.TEXTURE_2D, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataFormatType, this.memory_u8, ptr_data);
        if (bool_genMipMaps) {
            this.gl.generateMipmap(gl.TEXTURE_2D);
        }
        this.gl.bindTexture(this.gl.TEXTURE_2D, null);
    }

    wasmGraphics_TextureSetCubemap(int_glTextureId, int_glInternalFormat, int_width, int_height, int_glDataFormat, int_glDataType, ptr_rightData, ptr_leftData, ptr_topData, ptr_bottomData, ptr_backData, ptr_frontData, bool_genMipMaps) {
        let texture = this.textures[int_glTextureId];
        
        this.gl.bindTexture(this.gl.TEXTURE_2D, texture); 
    
        this.gl.texImage2D(this.gl.TEXTURE_CUBE_MAP_POSITIVE_X, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataType, this.memory_u8, ptr_rightData);
        this.gl.texImage2D(this.gl.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataType, this.memory_u8, ptr_leftData);
        this.gl.texImage2D(this.gl.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataType, this.memory_u8, ptr_topData);
        this.gl.texImage2D(this.gl.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataType, this.memory_u8, ptr_bottomData);
        this.gl.texImage2D(this.gl.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataType, this.memory_u8, ptr_backData);
        this.gl.texImage2D(this.gl.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, int_glInternalFormat, int_width, int_height, 0, int_glDataFormat, int_glDataType, this.memory_u8, ptr_frontData);
    
        if (bool_genMipMaps) {
            this.gl.generateMipmap(gl.TEXTURE_2D);
        }
        this.gl.bindTexture(this.gl.TEXTURE_2D, null);
    }

    wasmGraphics_DeviceSetFaceVisibility(bool_enableCullFace, bool_disableCullFace, int_cullFaceType, bool_changeFace, int_faceWind) {
        if (bool_enableCullFace) {
            this.gl.enable(this.gl.CULL_FACE);
            this.gl.cullFace(int_cullFaceType);
        }
        else if (bool_disableCullFace) {
            this.gl.disable(this.gl.CULL_FACE);
        }

        if (bool_changeFace) {
            this.gl.frontFace(int_faceWind)
        }
    }

    wasmGraphics_DeviceClearRGBAD(float_r, float_g, float_b, float_d) {
        this.gl.clearColor(float_r, float_g, float_b, 1.0);
        this.gl.clearDepth(float_d);
        this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);
    }

    wasmGraphics_SetDepthState(bool_changeDepthState, int_depthState, bool_changeDepthFunc, int_func, bool_changeDepthRange, float_depthRangeMin, float_depthRangeMax) {
        if (bool_changeDepthState) {
            if (int_depthState !== 0) {
                this.gl.enable(this.gl.DEPTH_TEST);
            }
            else {
                this.gl.disable(this.gl.DEPTH_TEST);
            }
        }
    
        if (bool_changeDepthFunc) {
            this.gl.depthFunc(int_func);
        }
    
        if (bool_changeDepthRange) {
            this.gl.depthRange(float_depthRangeMin, float_depthRangeMax);
        }
    }

    wasmGraphics_DeviceClearBufferBits(bool_color, bool_depth) {
        if (bool_color && !bool_depth) {
            this.gl.clear(this.gl.COLOR_BUFFER_BIT);
        }
        else if (!bool_color && bool_depth) {
            this.gl.clear(this.gl.DEPTH_BUFFER_BIT);
        }
        else if (bool_color && bool_depth) {
            this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);
        }
    }

    wasmGraphics_DeviceWriteMask(bool_r, bool_g, bool_b, bool_a, bool_depth) {
        this.gl.colorMask(bool_r, bool_g, bool_b, bool_a);
	    this.gl.depthMask(bool_depth);
    }

    wasmGraphics_SetGLBlendFuncEnabled(bool_state) {
        if (bool_state) {
            this.gl.enable(this.gl.BLEND);
        }
        else {
            this.gl.disable(this.gl.BLEND);
        }
    }

    wasmGraphics_UpdateGLBlendColor(float_r, float_g, float_b, float_a) {
		this.gl.blendColor(float_r, float_g, float_b, float_a);
    }

    wasmGraphics_ChangeGLBlendFuncSame(int_srcRgb, int_dstRgb) {
		this.gl.blendFunc(int_srcRgb, int_dstRgb);
    }

    wasmGraphics_ChangeGLBlendFuncSeperate(int_srcRgb, int_dstRgb, int_srcAlpha, int_dstAlpha) {
		this.gl.blendFuncSeparate(int_srcRgb, int_dstRgb, int_srcAlpha, int_dstAlpha);
    }

    wasmGraphics_ChangeGLBlendEquation(int_rgbEquation) {
        this.gl.blendEquation(int_rgbEquation);
    }

    wasmGraphics_ChangeGLBlendEquationSeparate(int_rgbEquation, int_alphaEquation) {
        this.gl.blendEquationSeparate(int_rgbEquation, int_alphaEquation);
    }

    wasmGraphics_DeviceClearColor(float_r, float_g, float_b) {
        this.gl.clearColor(float_r, float_g, float_b, 1.0);
	    this.gl.clear(this.gl.COLOR_BUFFER_BIT);
    }

    wasmGraphics_DeviceClearDepth(float_depth) {
        this.gl.clearDepth(float_depth);
	    this.gl.clear(this.gl.DEPTH_BUFFER_BIT);
    }

    wasmGraphics_SetGLViewport(int_x, int_y, int_w, int_h) {
        this.gl.viewport(int_x, int_y, int_w, int_h);
    }

    wasmGraphics_DeviceSetScissorState(bool_enableScissor, bool_disableScissor, bool_updateRect, int_x, int_y, int_w, int_h) {
        if (bool_enableScissor) {
            this.gl.enable(this.gl.SCISSOR_TEST);
        }
        else if (bool_disableScissor) {
            this.gl.disable(this.gl.SCISSOR_TEST);
        }

        if (bool_updateRect) {
		    this.gl.scissor(int_x, int_y, int_w, int_h);
        }
    }

    wasmGraphics_DeviceSetRenderTarget(int_frameBufferId, int_numAttachments) {
        if (int_frameBufferId != 0) {
            this.gl.bindFramebuffer(GL_FRAMEBUFFER, int_frameBufferId);

            if (int_numAttachments == 0) {
                this.gl.drawBuffer(this.gl.NONE);
                this.gl.readBuffer(this.gl.NONE);
            }
            else {
                let buffers = [
                    this.gl.COLOR_ATTACHMENT0, 
                    this.gl.COLOR_ATTACHMENT1,
                    this.gl.COLOR_ATTACHMENT2,
                    this.gl.COLOR_ATTACHMENT3,
                    this.gl.COLOR_ATTACHMENT4,
                    this.gl.COLOR_ATTACHMENT5,
                    this.gl.COLOR_ATTACHMENT6,
                    this.gl.COLOR_ATTACHMENT7,
                ];
                while(buffers.length > int_numAttachments && buffers.length != 0) {
                    buffers.pop();
                }

                this.gl.drawBuffers(buffers);
                this.gl.readBuffer(this.gl.FRONT);
            }
            
        }
        else {
            let buffers = [this.gl.BACK];
            this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
            this.gl.drawBuffers(buffers);
        }
    }

    wasmGraphics_DeviceDraw(int_glVao, bool_indexed, int_instanceCount, int_drawMode, int_startIndex, int_indexCount, int_bufferType) {
        this.gl.bindVertexArray(int_glVao);
        if (bool_indexed) {
            if (int_instanceCount <= 1) {
                this.gl.drawElements(int_drawMode, int_indexCount, int_bufferType, int_startIndex);
            }
            else {
                this.gl.drawElementsInstanced(int_drawMode, int_indexCount, int_bufferType, int_startIndex, int_instanceCount);
            }
        }
        else {
            if (int_instanceCount <= 1) {
                this.gl.drawArrays(int_drawMode, int_startIndex, int_indexCount);
            }
            else {
                this.gl.drawArraysInstanced(int_drawMode, int_startIndex, int_indexCount, int_instanceCount);
            }
        }
    }

    wasmGraphics_VertexLayoutSet(int_glVaoId, int_glBufferId, int_slotId, int_numComponents, int_type, int_stride, int_offset, int_divisor) {
        this.gl.bindVertexArray(int_glVaoId);
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, int_glBufferId);
        this.gl.vertexAttribPointer(int_slotId, int_numComponents, int_type, this.gl.FALSE, int_stride, int_offset);
        this.gl.enableVertexAttribArray(int_slotId);
        this.gl.vertexAttribDivisor(int_slotId, int_divisor);
        this.gl.bindVertexArray(null);
    }

    wasmGraphics_VertexLayoutSetIndexBuffer(int_glVaoId, int_glElementArrayBufferId) {
        this.gl.bindVertexArray(int_glVaoId);
        this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, int_glElementArrayBufferId);
        this.gl.bindVertexArray(null);
    }

    wasmGraphics_FramebufferAttachColor(int_attachTarget, int_frameBufferId, int_textureId, int_attachmentIndex) {
	    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, int_frameBufferId);
	    this.gl.bindTexture(int_attachTarget, int_textureId);
		this.gl.texParameteri(int_attachTarget, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
		this.gl.texParameteri(int_attachTarget, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
	    this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, this.gl.COLOR_ATTACHMENT0 + int_attachmentIndex, int_attachTarget, int_textureId, 0);
	    this.gl.bindTexture(int_attachTarget, null);
	    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
    }

    wasmGraphics_FrameBufferAttachDepth(int_attachTarget, int_frameBufferId, int_textureId, bool_pcm) {
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, int_frameBufferId);
	    this.gl.bindTexture(int_attachTarget, int_textureId);
        if (bool_pcm) {
            this.gl.texParameteri(int_attachTarget, this.gl.TEXTURE_COMPARE_MODE, this.gl.COMPARE_REF_TO_TEXTURE);
            this.gl.texParameteri(int_attachTarget, this.gl.TEXTURE_COMPARE_FUNC, this.gl.LEQUAL);
        }
        this.gl.texParameteri(int_attachTarget, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
		this.gl.texParameteri(int_attachTarget, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
	    this.gl.framebufferTexture2D(this.gl.FRAMEBUFFER, this.gl.DEPTH_ATTACHMENT, int_attachTarget, int_textureId, 0);
        this.gl.bindTexture(int_attachTarget, null);
	    this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
    }

    wasmGraphics_FrameBufferIsValid(int_frameBufferId) {
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, int_frameBufferId);
        let result = this.gl.checkFramebufferStatus(this.gl.FRAMEBUFFER) == this.gl.FRAMEBUFFER_COMPLETE;
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
        return result;
    }

    wasmGraphics_FrameBufferResolveTo(int_readBuffer, int_drawBuffer, int_x0, int_y0, int_x1, int_y1, int_x2, int_y2, int_x3, int_y3, bool_colorBit, bool_depthBit, int_filter) {
        this.gl.bindFramebuffer(this.gl.READ_FRAMEBUFFER, int_readBuffer);
        this.gl.bindFramebuffer(this.gl.DRAW_FRAMEBUFFER, int_drawBuffer);
        let mask = 0;
        if (bool_depthBit) {
		    mask |= this.gl.DEPTH_BUFFER_BIT;
        }
        if (bool_colorBit) {
            mask |= this.gl.COLOR_BUFFER_BIT;
        }
        this.gl.blitFramebuffer(int_x0, int_y0, int_x1, int_y1, int_x2, int_y2, int_x3, int_y3, mask, int_filter);
        this.gl.bindFramebuffer(this.gl.FRAMEBUFFER, null);
    }

    wasmGraphics_BufferReset(int_bufferId) {
        this.wasmGraphics_GLDestroyBuffer(this.int_bufferId);
	    return this.wasmGraphics_GLGenBuffer();
    }

    wasmGraphics_BufferSet(bool_indexBuffer, int_bufferId, int_arraySizeInByfes, ptr_inputArray, bool_isStatic) {
        if (bool_indexBuffer) {
            this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, int_bufferId);
            this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, this.memory_u8, this.gl.STATIC_DRAW, ptr_inputArray, int_arraySizeInByfes);
            this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, null);
        }
        else {
            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, int_bufferId);
            this.gl.bufferData(this.gl.ARRAY_BUFFER, this.memory_u8, _static? this.gl.STATIC_DRAW : this.gl.DYNAMIC_DRAW, ptr_inputArray, int_arraySizeInByfes);
            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, null);
        }
    }

    wasmGraphics_ShaderGetUniform(int_program, ptr_name, int_name_len) {
        const array = new Uint8Array(this.memory_u8, ptr_name, int_name_len);
        return this.gl.getUniformLocation(int_program, this.decoder.decode(array));
    }

    wasmGraphics_ShaderGetAttribute(int_program, ptr_name, int_name_len) {
        const array = new Uint8Array(this.memory_u8, ptr_name, int_name_len);
        return this.gl.getAttribLocation(int_program, this.decoder.decode(array));
    }

    wasmGraphics_ResetVertexLayout(int_id) {
        this.gl.BindVertexArray(null);
        this.gl.deleteVertexArrays(int_id);
        return this.gl.createVertexArray();
    }

    wasmGraphics_BindVAO(int_vaoId) {
        this.gl.bindVertexArray(int_vaoId);
    }

    wasmGraphics_DeviceBindTexture(int_textureUnit, int_textureUnitIndex, int_textureTarget, int_textureId, int_uniformSlot, int_minFilter, int_magFilter, int_wrapS, int_wrapT, int_wrapR, bool_updateSampler) {
	    this.gl.activeTexture(int_textureUnit);
        this.gl.bindTexture(int_textureTarget, int_textureId);

        if (bool_updateSampler) {
            this.gl.texParameteri(int_textureTarget, this.gl.TEXTURE_MIN_FILTER, int_minFilter);
            this.gl.texParameteri(int_textureTarget, this.gl.TEXTURE_MAG_FILTER, int_magFilter);
            this.gl.texParameteri(int_textureTarget, this.gl.TEXTURE_WRAP_S, int_wrapS);
            this.gl.texParameteri(int_textureTarget, this.gl.TEXTURE_WRAP_T, int_wrapT);
            this.gl.texParameteri(int_textureTarget, this.gl.TEXTURE_WRAP_R, int_wrapR);
        }

	    this.gl.uniform1i(int_uniformSlot, int_textureUnitIndex);
    }

    wasmGraphics_DeviceSetUniform(int_type, int_slotId, int_count, ptr_data) {
        if (int_type == 0/* UniformType::Int1 */) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(i32));
            this.gl.Uniform1iv(int_slotId, int_count, data);
        }
        else if (int_type == 1/*UniformType::Int2*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(i32) * 2);
            this.gl.uniform2iv(int_slotId, int_count, data);
        }
        else if (int_type == 2/*UniformType::Int3*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(i32) * 3);
            this.gl.uniform3iv(int_slotId, int_count, data);
        }
        else if (int_type == 3/*UniformType::Int4*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(i32) * 4);
            this.gl.uniform4iv(int_slotId, int_count, data);
        }
        else if (int_type == 4/*UniformType::Float1*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(f32));
            this.gl.uniform1fv(int_slotId, int_count, data);
        }
        else if (int_type == 5/*UniformType::Float2*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(f32) * 2);
            this.gl.uniform2fv(int_slotId, int_count, data);
        }
        else if (int_type == 6/*UniformType::Float3*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(f32) * 3);
            this.gl.uniform3fv(int_slotId, int_count, data);
        }
        else if (int_type == 7/*UniformType::Float4*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(f32) * 4);
            this.gl.uniform4fv(int_slotId, int_count, data);
        }
        else if (int_type == 8/*UniformType::Float9*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(f32) * 9);
            this.gl.uniformMatrix3fv(int_slotId, int_count, this.gl.FALSE, data);
        }
        else if (int_type == 9/*UniformType::Float16*/) {
            let data = new Uint8Array(this.memory_u8, ptr_data, int_count * sizeof(f32) * 16);
            this.gl.uniformMatrix4fv(int_slotId, int_count, this.gl.FALSE, data);
        }
    }

    wasmGraphics_DeviceBindShader(int_programId, int_boundTextures) {
		this.gl.useProgram(int_programId);
        for (let i = 0; i < 32; ++i) {
            let set = int_boundTextures & (1 << i);
            if (set) {
                this.gl.activeTexture(this.gl.TEXTURE0 + i);
                this.gl.bindTexture(this.gl.TEXTURE_CUBE_MAP, null);
                this.gl.bindTexture(this.gl.TEXTURE_2D, null);
            }
        }
        this.gl.activeTexture(this.gl.TEXTURE0);
    }

    wasmGraphics_SetDefaultGLState() {
        this.gl.disable(this.gl.BLEND);
        this.gl.blendColor(0.0, 0.0, 0.0, 0.0);
        this.gl.blendFunc(this.gl.ONE, this.gl.ZERO);
        this.gl.blendEquation(this.gl.FUNC_ADD);
        this.gl.enable(this.gl.CULL_FACE);
        this.gl.cullFace(this.gl.BACK);
        this.gl.frontFace(this.gl.CCW);
        this.gl.enable(this.gl.DEPTH_TEST);
        this.gl.depthFunc(this.gl.LESS);
        this.gl.depthRange(0.0, 1.0);
        this.gl.disable(this.gl.SCISSOR_TEST);
        this.gl.scissor(0, 0, 800, 600);
    }

    wasmGraphics_GetScissorAndViewport(ptr_scissorPtr, ptr_viewPtr) {
        let scissorRect = this.gl.getParameter(this.gl.SCISSOR_BOX);
        let viewportRect = this.gl.getParameter(this.gl.VIEWPORT);

        let targetScissor = new Uint32Array(this.memory.buffer, ptr_scissorPtr, 4 * 4); ;
        let targetView =new Uint32Array(this.memory.buffer, ptr_viewPtr, 4 * 4);

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
        const vertexShaderStr = this.decoder.decode(new Uint8Array(this.memory.buffer, ptr_vShader, int_vShaderLen));
        const fragmentShaderStr = this.decoder.decode(new Uint8Array(this.memory.buffer, ptr_fShader, int_fShaderLen));
        
        let vertexShader = this.gl.createShader(this.gl.VERTEX_SHADER);
        this.gl.shaderSource(vertexShader, vertexShaderStr);
        this.gl.compileShader(vertexShader);
        let success = this.gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS);
        if (!success) {
            let message = gl.getShaderInfoLog(vertexShader);
            this.log("Error compiling vertex shader: " + message);
            return 0;
        }

        let fragmentShader = this.gl.createShader(this.gl.FRAGMENT_SHADER);
        this.gl.shaderSource(fragmentShader, fragmentShaderStr);
        this.gl.compileShader(fragmentShader);
        success = this.gl.getShaderParameter(vertexShader, gl.COMPILE_STATUS);
        if (!success) {
            let message = gl.getShaderInfoLog("Error compiling fragment shader: " + fragmentShader);
            this.log(message);
            return 0;
        }

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
        
        // TODO:
        // Now shove that shader into an array
        // and return its index
    }

    wasmGraphics_GLDestroyShader(int_shaderId) {
        // TODO
    }
};