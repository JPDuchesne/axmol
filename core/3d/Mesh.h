/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#pragma once

#include <string>
#include <map>

#include "3d/Bundle3DData.h"
#include "3d/AABB.h"
#include "3d/3DProgramInfo.h"

#include "base/Ref.h"
#include "math/Math.h"
#include "renderer/MeshCommand.h"
#include "renderer/CustomCommand.h"

NS_AX_BEGIN

/**
 * @addtogroup _3d
 * @{
 */

class Texture2D;
class MeshSkin;
class MeshIndexData;
class Material;
class Renderer;
class Scene;
class Pass;

namespace backend
{
class Buffer;
}

/**
 * @brief Mesh: contains ref to index buffer, GLProgramState, texture, skin, blend function, aabb and so on
 */
class AX_DLL Mesh : public Ref
{
    friend class MeshRenderer;

public:

    /**create mesh from positions, normals, and so on, single SubMesh*/
    static Mesh* create(const std::vector<float>& positions,
                        const std::vector<float>& normals,
                        const std::vector<float>& texs,
                        const IndexArray& indices);

    /**
     * @lua NA
     */
    static Mesh* create(const std::vector<float>& vertices,
                        int perVertexSizeInFloat,
                        const IndexArray& indices,
                        const std::vector<MeshVertexAttrib>& attribs);

    /**
     * create mesh
     * @lua NA
     */
    static Mesh* create(std::string_view name, MeshIndexData* indexData, MeshSkin* skin = nullptr);

    /**
     * get vertex buffer
     *
     * @lua NA
     */
    backend::Buffer* getVertexBuffer() const;
    /**
     * has vertex attribute?
     *
     * @lua NA
     */
    bool hasVertexAttrib(shaderinfos::VertexKey attrib) const;
    /**get mesh vertex attribute count*/
    ssize_t getMeshVertexAttribCount() const;
    /**get MeshVertexAttribute by index*/
    const MeshVertexAttrib& getMeshVertexAttribute(int idx);
    /**get per vertex size in bytes*/
    int getVertexSizeInBytes() const;

    /**
     * set texture (diffuse), which is responsible for the main appearance. It is also means main texture, you can also
     * call setTexture(texPath, NTextureData::Usage::Diffuse)
     * @param texPath texture path
     */
    void setTexture(std::string_view texPath);
    /**
     * set texture (diffuse), which is responsible for the main appearance. It is also means main texture, you can also
     * call setTexture(texPath, NTextureData::Usage::Diffuse)
     * @param tex texture to be set
     */
    void setTexture(Texture2D* tex);
    /**
     * set texture
     * @param tex texture to be set
     * @param usage Usage of this texture
     * @param whether refresh the cache file name
     */
    void setTexture(Texture2D* tex, NTextureData::Usage usage, bool cacheFileName = true);
    /**
     * set texture
     * @param texPath texture path
     * @param usage Usage of this texture
     */
    void setTexture(std::string_view texPath, NTextureData::Usage usage);
    /**
     * Get texture (diffuse), which is responsible for the main appearance. It is also means main texture, you can also
     * call getTexture(NTextureData::Usage::Diffuse)
     * @return Texture used, return the texture of first mesh if multiple meshes exist
     */
    Texture2D* getTexture() const;
    /**
     * Get texture
     * @param usage Usage of returned texture
     * @return The texture of this usage, return the texture of first mesh if multiple meshes exist
     */
    Texture2D* getTexture(NTextureData::Usage usage);

    /**visible getter and setter*/
    void setVisible(bool visible);
    bool isVisible() const;

    /**
     * skin getter
     *
     * @lua NA
     */
    MeshSkin* getSkin() const { return _skin; }

    /**
     * mesh index data getter
     *
     * @lua NA
     */
    MeshIndexData* getMeshIndexData() const { return _meshIndexData; }

    /**
     * get ProgramState
     *
     * @lua NA
     */
    backend::ProgramState* getProgramState() const;

    /**name getter */
    std::string_view getName() const { return _name; }

    void setBlendFunc(const BlendFunc& blendFunc);
    const BlendFunc& getBlendFunc() const;

    /**
     * get primitive type
     *
     * @lua NA
     */
    CustomCommand::PrimitiveType getPrimitiveType() const;
    /**
     * get index count
     *
     * @lua NA
     */
    ssize_t getIndexCount() const;
    /**
     * get index format
     *
     * @lua NA
     */
    CustomCommand::IndexFormat getIndexFormat() const;
    /**
     * set index format
     *
     * @lua NA
     */
    void setIndexFormat(CustomCommand::IndexFormat indexFormat);
    /**
     * get index buffer
     *
     * @lua NA
     */
    backend::Buffer* getIndexBuffer() const;

    /**get AABB*/
    const AABB& getAABB() const { return _aabb; }

    /**  Sets a new ProgramState for the Mesh
     * A new Material will be created for it
     */
    void setProgramState(backend::ProgramState* programState);

    /** Sets a new Material to the Mesh */
    void setMaterial(Material* material);

    /** Returns the Material being used by the Mesh */
    Material* getMaterial() const;

    void draw(Renderer* renderer,
              float globalZ,
              const Mat4& transform,
              uint32_t flags,
              unsigned int lightMask,
              const Vec4& color,
              bool forceDepthWrite,
              bool wireframe);

    /**skin setter*/
    void setSkin(MeshSkin* skin);
    /**Mesh index data setter*/
    void setMeshIndexData(MeshIndexData* indexdata);
    /**name setter*/
    void setName(std::string_view name) { _name = name; }

    /**
     * calculate the AABB of the mesh
     * @note the AABB is in the local space, not the world space
     */
    void calculateAABB();

    std::string getTextureFileName() { return _texFile; }

    Mesh();
    virtual ~Mesh();

protected:
    void resetLightUniformValues();
    void setLightUniforms(Pass* pass, Scene* scene, const Vec4& color, unsigned int lightmask);
    void bindMeshCommand();

    std::map<NTextureData::Usage, Texture2D*> _textures;  // textures that submesh is using
    MeshSkin* _skin;                                      // skin
    bool _visible;                                        // is the submesh visible

    CustomCommand::IndexFormat meshIndexFormat;

    std::string _name;
    MeshIndexData* _meshIndexData;
    // GLProgramState*     _glProgramState;
    BlendFunc _blend;
    bool _blendDirty;
    Material* _material;
    AABB _aabb;
    std::function<void()> _visibleChanged;
    std::unordered_map<std::string, std::vector<MeshCommand>> _meshCommands;

    /// light parameters
    std::vector<Vec3> _dirLightUniformColorValues;
    std::vector<Vec3> _dirLightUniformDirValues;

    std::vector<Vec3> _pointLightUniformColorValues;
    std::vector<Vec3> _pointLightUniformPositionValues;
    std::vector<float> _pointLightUniformRangeInverseValues;

    std::vector<Vec3> _spotLightUniformColorValues;
    std::vector<Vec3> _spotLightUniformPositionValues;
    std::vector<Vec3> _spotLightUniformDirValues;
    std::vector<float> _spotLightUniformInnerAngleCosValues;
    std::vector<float> _spotLightUniformOuterAngleCosValues;
    std::vector<float> _spotLightUniformRangeInverseValues;

    std::string _texFile;
};

// end of 3d group
/// @}

/// @cond
extern std::string AX_DLL s_uniformSamplerName[];  // uniform sampler names array
/// @endcond

NS_AX_END
