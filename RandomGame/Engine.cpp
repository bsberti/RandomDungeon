#include "Engine.h"

#include <vector>
#include <assert.h>
#include <sstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "AnimationManager.h"
#include "GraphicScene.h"
#include "vertex_types.h"

#include "Animation.h"
#include "BoneHierarchy.h"
#include "cCharacter.h"
#include "BoneShader.h"

DebugRendererObject DebugBone;

// Managers
AnimationManager g_AnimationManager;

Assimp::Importer g_Importer;

bool RenderObjects = true;
bool RenderPhysicsDebug;

float g_PrevTime;
float g_CurrTime;

const int NUM_BONES = 52;
struct BoneShader {
    GLuint ProjectionMatrix;
    GLuint ViewMatrix;
    GLuint ModelMatrix;
    GLuint RotationMatrix;
    GLuint BoneMatrices[NUM_BONES];
    GLuint BoneRotationMatrices[NUM_BONES];

    GLuint Texture00;
    GLuint Color;
} gBoneShader;

struct Material {
    unsigned int TextureId;
    glm::vec3 Color;
};

struct LineShader {
    GLuint ProjectionMatrix;
    GLuint ViewMatrix;
} gLineShader;

unsigned int gBoneShaderId;
unsigned int gSimpleShaderId;
unsigned int gLineShaderId;

std::vector<Model*> gModelVec;
std::vector<Material*> gMaterialVec;
extern GraphicScene g_GraphicScene;

cCharacter* g_tmpCharacter;
cMeshObject* g_tmpCharacterGameObject;

void (*Updatecallback)(float dt);
void (*Rendercallback)(void);

void BoneShaderUpdate(unsigned int shaderID) {
    gBoneShader.ModelMatrix = glGetUniformLocation(shaderID, "ModelMatrix");
    gBoneShader.ViewMatrix = glGetUniformLocation(shaderID, "ViewMatrix");
    gBoneShader.ProjectionMatrix = glGetUniformLocation(shaderID, "ProjectionMatrix");
    gBoneShader.RotationMatrix = glGetUniformLocation(shaderID, "RotationMatrix");
    gBoneShader.Texture00 = glGetUniformLocation(shaderID, "Texture00");
    gBoneShader.Color = glGetUniformLocation(shaderID, "Color");

    for (int i = 0; i < NUM_BONES; i++)
    {
        std::stringstream ssBoneMatrices;
        ssBoneMatrices << "BoneMatrices[" << i << "]";
        gBoneShader.BoneMatrices[i] = glGetUniformLocation(shaderID, ssBoneMatrices.str().c_str());

        std::stringstream ssBoneRotationMatrices;
        ssBoneRotationMatrices << "BoneRotationMatrices[" << i << "]";
        gBoneShader.BoneRotationMatrices[i] = glGetUniformLocation(shaderID, ssBoneRotationMatrices.str().c_str());

    }
    //CheckGLError();
}

void CastToGLM(const aiMatrix4x4& in, glm::mat4& out)
{
    out = glm::mat4(in.a1, in.b1, in.c1, in.d1,
                    in.a2, in.b2, in.c2, in.d2,
                    in.a3, in.b3, in.c3, in.d3,
                    in.a4, in.b4, in.c4, in.d4);
}

Model* GDP_GetModel(unsigned int id) {
    return gModelVec[id];
}

Material* GetMaterial(unsigned int id) {
    return gMaterialVec[id];
}

cMeshObject* GDP_CreateGameObject() {
    cMeshObject* go = new cMeshObject();
    go->position = glm::vec3(0.0f);
    go->scaleXYZ = glm::vec3(1.0f);
    go->qRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    //if (addToWorld)
    //    g_GraphicScene.vec_pMeshCurrentMaze.push_back(go);

    return go;
}

bool LoadAssimpMesh(CharacterAnimationData& animationData, unsigned int& id, const aiMesh* mesh)
{
    if (mesh == nullptr)
        return false;
    //printf("LoadAssimpMesh %s\n", mesh->mName.C_Str());

    unsigned int numFaces = mesh->mNumFaces;

    unsigned int numIndicesInIndexArray = numFaces * 3;

    sVertex_p4t4n4b4w4* pTempVertArray = new sVertex_p4t4n4b4w4[mesh->mNumVertices];
    GLuint* pIndexArrayLocal = new GLuint[numIndicesInIndexArray];

    // Create a bone reference map
    int totalWeights = 0;

    std::vector<BoneVertexData> boneVertexData;
    boneVertexData.resize(mesh->mNumVertices);
    int boneCount = 0;
    for (int i = 0; i < mesh->mNumBones; i++)
    {
        /*************** START Portion A *******************/
        aiBone* bone = mesh->mBones[i];

        int boneIdx = 0;
        std::string boneName(bone->mName.data);

        std::map<std::string, int>::iterator it = animationData.boneNameToIdMap.find(boneName);
        if (it == animationData.boneNameToIdMap.end())
        {
            boneIdx = boneCount;
            boneCount++;
            BoneInfo bi;
            bi.name = boneName;
            animationData.boneInfoVec.push_back(bi);

            CastToGLM(bone->mOffsetMatrix, animationData.boneInfoVec[boneIdx].boneOffset);
            animationData.boneNameToIdMap[boneName] = boneIdx;
        }
        else
        {
            boneIdx = animationData.boneNameToIdMap[boneName];
        }
        /*************** END Portion A *******************/


        /*************** START Portion B *******************/
        for (int weightIdx = 0; weightIdx < bone->mNumWeights; weightIdx++)
        {
            float weight = bone->mWeights[weightIdx].mWeight;
            int vertexId = bone->mWeights[weightIdx].mVertexId;
            boneVertexData[vertexId].AddBoneInfo(boneIdx, weight);
        }
        /*************** END Portion B *******************/
    }

    std::vector<unsigned int> triangles;
    int vertArrayIndex = 0;
    sModelDrawInfo drawInfo;

    drawInfo.numberOfVertices = mesh->mNumVertices;
    drawInfo.numberOfTriangles = numFaces;
    drawInfo.numberOfIndices = drawInfo.numberOfTriangles * 3;

    drawInfo.pVertices = new sVertex_RGBA_XYZ_N_UV_T_BiN_Bones[drawInfo.numberOfVertices];
    drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

    for (unsigned int vertArrayIndex = 0; vertArrayIndex < mesh->mNumVertices; vertArrayIndex++)
    {
        const aiVector3D& vertex = mesh->mVertices[vertArrayIndex];

        pTempVertArray[vertArrayIndex].Pos.x = vertex.x;
        pTempVertArray[vertArrayIndex].Pos.y = vertex.y;
        pTempVertArray[vertArrayIndex].Pos.z = vertex.z;
        pTempVertArray[vertArrayIndex].Pos.w = 1.0f;

        drawInfo.pVertices[vertArrayIndex].x = vertex.x;
        drawInfo.pVertices[vertArrayIndex].y = vertex.y;
        drawInfo.pVertices[vertArrayIndex].y = vertex.z;

        if (mesh->HasNormals())
        {
            const aiVector3D& normal = mesh->mNormals[vertArrayIndex];
            pTempVertArray[vertArrayIndex].Normal.x = normal.x;
            pTempVertArray[vertArrayIndex].Normal.y = normal.y;
            pTempVertArray[vertArrayIndex].Normal.z = normal.z;
            pTempVertArray[vertArrayIndex].Normal.w = 1.f;

            drawInfo.pVertices[vertArrayIndex].nx = normal.x;
            drawInfo.pVertices[vertArrayIndex].ny = normal.y;
            drawInfo.pVertices[vertArrayIndex].nz = normal.z;
            drawInfo.pVertices[vertArrayIndex].nw = 0.f;
        }
        else
        {
            pTempVertArray[vertArrayIndex].Normal.x = 1.f;
            pTempVertArray[vertArrayIndex].Normal.y = 0.f;
            pTempVertArray[vertArrayIndex].Normal.z = 0.f;
            pTempVertArray[vertArrayIndex].Normal.w = 0.f;

            drawInfo.pVertices[vertArrayIndex].nx = 1.f;
            drawInfo.pVertices[vertArrayIndex].ny = 0.f;
            drawInfo.pVertices[vertArrayIndex].nz = 0.f;
            drawInfo.pVertices[vertArrayIndex].nw = 0.f;
        }

        if (mesh->HasTextureCoords(0))
        {
            const aiVector3D& uvCoord = mesh->mTextureCoords[0][vertArrayIndex];
            pTempVertArray[vertArrayIndex].TexUVx2.x = uvCoord.x;
            pTempVertArray[vertArrayIndex].TexUVx2.y = uvCoord.y;

            // ----------------------------------------------------

            drawInfo.pVertices[vertArrayIndex].u0 = uvCoord.x;
            drawInfo.pVertices[vertArrayIndex].v0 = uvCoord.y;
        }

        if (mesh->HasTextureCoords(1))
        {
            const aiVector3D& uvCoord = mesh->mTextureCoords[1][vertArrayIndex];
            pTempVertArray[vertArrayIndex].TexUVx2.z = uvCoord.x;
            pTempVertArray[vertArrayIndex].TexUVx2.w = uvCoord.y;

            // ----------------------------------------------------

            drawInfo.pVertices[vertArrayIndex].u0 = uvCoord.x;
            drawInfo.pVertices[vertArrayIndex].v0 = uvCoord.y;
        }

        // Use a BoneInformation Map to get bone info and store the values here
        BoneVertexData& bvd = boneVertexData[vertArrayIndex];
        pTempVertArray[vertArrayIndex].BoneIds.x = bvd.ids[0];
        pTempVertArray[vertArrayIndex].BoneIds.y = bvd.ids[1];
        pTempVertArray[vertArrayIndex].BoneIds.z = bvd.ids[2];
        pTempVertArray[vertArrayIndex].BoneIds.w = bvd.ids[3];

        pTempVertArray[vertArrayIndex].BoneWeights.x = bvd.weights[0];
        pTempVertArray[vertArrayIndex].BoneWeights.y = bvd.weights[1];
        pTempVertArray[vertArrayIndex].BoneWeights.z = bvd.weights[2];
        pTempVertArray[vertArrayIndex].BoneWeights.w = bvd.weights[3];
    }

    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            pIndexArrayLocal[j] = face.mIndices[j];
            drawInfo.pIndices[j] = face.mIndices[j];
        }
    }

    //for (int faceIndex = 0; faceIndex < numFaces; faceIndex++)
    //{
    //    const aiFace& face = mesh->mFaces[faceIndex];
    //
    //    // You can assert here to ensure mNumIndices is 3
    //    // Unless you support non 3 face models 
    //    for (int indicesIndex = 0; indicesIndex < face.mNumIndices; indicesIndex++)
    //    {
    //        unsigned int vertexIndex = face.mIndices[indicesIndex];
    //        triangles.push_back(vertexIndex);
    //
    //        //printf("%d\t", vertexIndex);
    //
    //        const aiVector3D& vertex = mesh->mVertices[vertexIndex];
    //        drawInfo.pIndices[vertexIndex] = face.mIndices[indicesIndex];
    //
    //        pTempVertArray[vertArrayIndex].Pos.x = vertex.x;
    //        pTempVertArray[vertArrayIndex].Pos.y = vertex.y;
    //        pTempVertArray[vertArrayIndex].Pos.z = vertex.z;
    //        pTempVertArray[vertArrayIndex].Pos.w = 1.0f;
    //
    //        // ----------------------------------------------------
    //
    //        drawInfo.pVertices[vertArrayIndex].x = vertex.x;
    //        drawInfo.pVertices[vertArrayIndex].y = vertex.y;
    //        drawInfo.pVertices[vertArrayIndex].y = vertex.z;
    //
    //        if (mesh->HasNormals())
    //        {
    //            const aiVector3D& normal = mesh->mNormals[vertexIndex];
    //            pTempVertArray[vertArrayIndex].Normal.x = normal.x;
    //            pTempVertArray[vertArrayIndex].Normal.y = normal.y;
    //            pTempVertArray[vertArrayIndex].Normal.z = normal.z;
    //            pTempVertArray[vertArrayIndex].Normal.w = 0.f;
    //
    //            // ----------------------------------------------------
    //
    //            drawInfo.pVertices[vertArrayIndex].nx = normal.x;
    //            drawInfo.pVertices[vertArrayIndex].ny = normal.y;
    //            drawInfo.pVertices[vertArrayIndex].nz = normal.z;
    //            drawInfo.pVertices[vertArrayIndex].nw = 0.f;
    //        }
    //        else
    //        {
    //            pTempVertArray[vertArrayIndex].Normal.x = 1.f;
    //            pTempVertArray[vertArrayIndex].Normal.y = 0.f;
    //            pTempVertArray[vertArrayIndex].Normal.z = 0.f;
    //            pTempVertArray[vertArrayIndex].Normal.w = 0.f;
    //
    //            // ----------------------------------------------------
    //
    //            drawInfo.pVertices[vertArrayIndex].nx = 1.f;
    //            drawInfo.pVertices[vertArrayIndex].ny = 0.f;
    //            drawInfo.pVertices[vertArrayIndex].nz = 0.f;
    //            drawInfo.pVertices[vertArrayIndex].nw = 0.f;
    //        }
    //
    //        if (mesh->HasTextureCoords(0))
    //        {
    //            const aiVector3D& uvCoord = mesh->mTextureCoords[0][vertexIndex];
    //            pTempVertArray[vertArrayIndex].TexUVx2.x = uvCoord.x;
    //            pTempVertArray[vertArrayIndex].TexUVx2.y = uvCoord.y;
    //
    //            // ----------------------------------------------------
    //
    //            drawInfo.pVertices[vertArrayIndex].u0 = uvCoord.x;
    //            drawInfo.pVertices[vertArrayIndex].v0 = uvCoord.y;
    //        }
    //
    //        if (mesh->HasTextureCoords(1))
    //        {
    //            const aiVector3D& uvCoord = mesh->mTextureCoords[1][vertexIndex];
    //            pTempVertArray[vertArrayIndex].TexUVx2.z = uvCoord.x;
    //            pTempVertArray[vertArrayIndex].TexUVx2.w = uvCoord.y;
    //
    //            // ----------------------------------------------------
    //
    //            drawInfo.pVertices[vertArrayIndex].u0 = uvCoord.x;
    //            drawInfo.pVertices[vertArrayIndex].v0 = uvCoord.y;
    //        }
    //
    //        // Use a BoneInformation Map to get bone info and store the values here
    //        BoneVertexData& bvd = boneVertexData[vertexIndex];
    //        pTempVertArray[vertArrayIndex].BoneIds.x = bvd.ids[0];
    //        pTempVertArray[vertArrayIndex].BoneIds.y = bvd.ids[1];
    //        pTempVertArray[vertArrayIndex].BoneIds.z = bvd.ids[2];
    //        pTempVertArray[vertArrayIndex].BoneIds.w = bvd.ids[3];
    //
    //        pTempVertArray[vertArrayIndex].BoneWeights.x = bvd.weights[0];
    //        pTempVertArray[vertArrayIndex].BoneWeights.y = bvd.weights[1];
    //        pTempVertArray[vertArrayIndex].BoneWeights.z = bvd.weights[2];
    //        pTempVertArray[vertArrayIndex].BoneWeights.w = bvd.weights[3];
    //
    //        pIndexArrayLocal[vertArrayIndex] = vertArrayIndex;
    //
    //        vertArrayIndex++;
    //    }
    //}
    ////printf("  - Done loading Model data");
    //
    //unsigned int vertex_element_index_index = 0;
    //
    //for (unsigned int triangleIndex = 0; triangleIndex != drawInfo.numberOfTriangles; triangleIndex++)
    //{
    //    
    //    // Each +1 of the triangle index moves the "vertex element index" by 3
    //    // (3 vertices per triangle)
    //    vertex_element_index_index += 3;
    //}

    Model* model = new Model();
    int testNumTriangles = triangles.size();
    model->NumTriangles = numFaces;
    //glGenVertexArrays(1, &model->Vbo);
    //glBindVertexArray(model->Vbo);
    ////CheckGLError();

    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    //glEnableVertexAttribArray(2);
    //glEnableVertexAttribArray(3);
    //glEnableVertexAttribArray(4);
    //glEnableVertexAttribArray(5);
    ////CheckGLError();

    //glGenBuffers(1, &model->VertexBufferId);
    //glGenBuffers(1, &model->IndexBufferId);
    ////CheckGLError();

    //glBindBuffer(GL_ARRAY_BUFFER, model->VertexBufferId);
    ////CheckGLError();

    //unsigned int totalVertBufferSizeBYTES = drawInfo.numberOfVertices * sizeof(sVertex_p4t4n4b4w4);
    //glBufferData(GL_ARRAY_BUFFER, totalVertBufferSizeBYTES, pTempVertArray, GL_STATIC_DRAW);
    ////CheckGLError();

    //unsigned int bytesInOneVertex = sizeof(sVertex_p4t4n4b4w4);
    //unsigned int byteOffsetToPosition = offsetof(sVertex_p4t4n4b4w4, Pos);
    //unsigned int byteOffsetToNormal = offsetof(sVertex_p4t4n4b4w4, Normal);
    //unsigned int byteOffsetToUVCoords = offsetof(sVertex_p4t4n4b4w4, TexUVx2);
    //unsigned int byteOffsetToBoneWeights = offsetof(sVertex_p4t4n4b4w4, BoneWeights);
    //unsigned int byteOffsetToBoneIds = offsetof(sVertex_p4t4n4b4w4, BoneIds);

    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToPosition);
    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToUVCoords);
    //glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToNormal);
    //glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToBoneWeights);
    //glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, bytesInOneVertex, (GLvoid*)byteOffsetToBoneIds);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->IndexBufferId);

    //unsigned int sizeOfIndexArrayInBytes = numIndicesInIndexArray * sizeof(GLuint);

    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndexArrayInBytes, pIndexArrayLocal, GL_STATIC_DRAW);

    //glBindVertexArray(0);

    delete[] pTempVertArray;
    delete[] pIndexArrayLocal;

    //printf("  - Done Generating Buffer data");

    //std::string friendlyName = "AnimatedChar"; // TO-DO CHANGE THAT
    std::string friendlyName = mesh->mName.C_Str(); // TO-DO CHANGE THAT
    drawInfo.meshName = mesh->mName.C_Str();

    if (g_GraphicScene.pVAOManager->LoadModelIntoVAO(friendlyName, drawInfo, 3)) {
        std::cout << "Loaded the " << friendlyName << " model" << std::endl;
    }

    //g_GraphicScene.pVAOManager->AddDrawInfoToMap(friendlyName, drawInfo);

    printf("  - Finished Loading model \"%s\" with %d vertices, %d triangles, id is: %d\n", mesh->mName.C_Str(), model->Vertices.size(), model->NumTriangles, model->Vbo);
    id = gModelVec.size();
    gModelVec.push_back(model);
    return true;
}

void LoadAssimpAnimation(CharacterAnimationData& characterAnimation, const aiAnimation* animation, BoneHierarchy* boneHierarchy)
{
    if (animation == nullptr)
        return;

    printf("LoadAssimpAnimation %s\n", animation->mName.C_Str());

    characterAnimation.Name = std::string(animation->mName.C_Str());
    characterAnimation.Duration = animation->mDuration;
    characterAnimation.TicksPerSecond = animation->mTicksPerSecond;

    unsigned int numChannels = animation->mNumChannels;

    printf("- Loading %d channels\n", numChannels);

    characterAnimation.Channels.resize(numChannels);
    for (int i = 0; i < numChannels; i++)
    {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        std::string name(nodeAnim->mNodeName.C_Str());

        printf("  %s\n", name.c_str());

        unsigned int numPositionKeys = nodeAnim->mNumPositionKeys;
        unsigned int numRotationKeys = nodeAnim->mNumRotationKeys;
        unsigned int numScalingKeys = nodeAnim->mNumScalingKeys;

        AnimationData* animData = new AnimationData();
        characterAnimation.Channels[i] = animData;
        animData->Name = name;

        animData->PositionKeyFrames.resize(numPositionKeys);
        animData->RotationKeyFrames.resize(numRotationKeys);
        animData->ScaleKeyFrames.resize(numScalingKeys);

        for (int keyIdx = 0; keyIdx < numPositionKeys; keyIdx++)
        {
            const aiVectorKey& posKey = nodeAnim->mPositionKeys[keyIdx];
            animData->PositionKeyFrames[keyIdx].time = posKey.mTime;
            animData->PositionKeyFrames[keyIdx].value.x = posKey.mValue.x;
            animData->PositionKeyFrames[keyIdx].value.y = posKey.mValue.y;
            animData->PositionKeyFrames[keyIdx].value.z = posKey.mValue.z;
        }

        for (int keyIdx = 0; keyIdx < numRotationKeys; keyIdx++)
        {
            const aiQuatKey& rotKey = nodeAnim->mRotationKeys[keyIdx];
            animData->RotationKeyFrames[keyIdx].time = rotKey.mTime;
            animData->RotationKeyFrames[keyIdx].value.x = rotKey.mValue.x;
            animData->RotationKeyFrames[keyIdx].value.y = rotKey.mValue.y;
            animData->RotationKeyFrames[keyIdx].value.z = rotKey.mValue.z;
            animData->RotationKeyFrames[keyIdx].value.w = rotKey.mValue.w;
        }

        for (int keyIdx = 0; keyIdx < numScalingKeys; keyIdx++)
        {
            const aiVectorKey& scaleKey = nodeAnim->mScalingKeys[keyIdx];
            animData->ScaleKeyFrames[keyIdx].time = scaleKey.mTime;
            animData->ScaleKeyFrames[keyIdx].value.x = scaleKey.mValue.x;
            animData->ScaleKeyFrames[keyIdx].value.y = scaleKey.mValue.y;
            animData->ScaleKeyFrames[keyIdx].value.z = scaleKey.mValue.z;
        }
    }

    characterAnimation.BoneHierarchy = boneHierarchy;
    g_AnimationManager.LoadCharacterAnimation(characterAnimation.Name, characterAnimation);

    printf("- Done!\n");
}

BoneNode* CreateNodeHierarchy(aiNode* node, int depth = 0)
{
    BoneNode* newNode = new BoneNode();
    newNode->name = node->mName.C_Str();
    CastToGLM(node->mTransformation, newNode->transformation);

    for (int i = 0; i < depth; i++)
        printf(" ");
    printf("%s (%d)\n", newNode->name.c_str(), node->mNumChildren);


    glm::vec3 globalScale, globalTranslation, ignore3;
    glm::vec4 ignore4;
    glm::quat globalOrientation;

    bool success = glm::decompose(newNode->transformation, globalScale, globalOrientation, globalTranslation, ignore3, ignore4);

    printf("  NodeTransformation:\n");
    printf("    Position: (%.4f, %.4f, %.4f)\n", globalTranslation.x, globalTranslation.y, globalTranslation.z);
    printf("    Scale: (%.4f, %.4f, %.4f)\n", globalScale.x, globalScale.y, globalScale.z);
    printf("    Rotation: (%.4f, %.4f, %.4f, %.4f)\n", globalOrientation.x, globalOrientation.y, globalOrientation.z, globalOrientation.w);
    printf("\n");

    //const aiMatrix4x4& m = node->mTransformation;
    //printf("%.2f %.2f %.2f %.2f, %.2f %.2f %.2f %.2f, %.2f %.2f %.2f %.2f, %.2f %.2f %.2f %.2f\n",
    //    m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3, m.c3, m.d3, m.a4, m.b4, m.c4, m.d4);

    std::vector<aiNode*> children;

    for (int i = 0; i < node->mNumChildren; i++)
    {
        children.push_back(node->mChildren[i]);

        BoneNode* childNode = CreateNodeHierarchy(node->mChildren[i], depth + 1);
        newNode->AddChild(childNode);
    }

    return newNode;
}

void GDP_LoadFBXFile(unsigned int& meshId, std::string& animationName, const std::string& filename)
{
    unsigned int Flags = aiProcess_Triangulate 
        | aiProcess_JoinIdenticalVertices;
    const aiScene* scene = g_Importer.ReadFile(filename, Flags);

    printf("GDP_LoadFBXFile: %s\n", filename.c_str());

    // Create our Bone Hierarchy
    BoneHierarchy* boneHierarchy = new BoneHierarchy();

    // Node hierarchy for rendering
    boneHierarchy->root = CreateNodeHierarchy(scene->mRootNode);
    CastToGLM(scene->mRootNode->mTransformation, boneHierarchy->globalInverseTransform);
    boneHierarchy->globalInverseTransform = glm::inverse(boneHierarchy->globalInverseTransform);

    if (scene->HasMeshes())
    {
        CharacterAnimationData animationData(scene);

        // Load all the meshes
        unsigned int numMeshes = scene->mNumMeshes;
        printf("-Loading %d meshes!\n", numMeshes);
        //for (int i = 0; i < numMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[0];
            if (!LoadAssimpMesh(animationData, meshId, mesh))
            {
                printf("Failed to load mesh! \n");
            }

            //break;
        }

        printf("\n");

        unsigned int numAnimations = scene->mNumAnimations;
        printf("-Loading %d animations!\n", numAnimations);
        for (int i = 0; i < numAnimations; i++)
        {
            aiAnimation* animation = scene->mAnimations[i];

            animationName = animation->mName.C_Str();
            LoadAssimpAnimation(animationData, animation, boneHierarchy);
        }
    }

    printf("Done loading FBX file!\n");
} 

cMeshObject* GDP_CreateAnimatedCharacter(const char* filename,
    const std::vector<std::string>& animations)
{
    g_tmpCharacterGameObject = GDP_CreateGameObject();
    g_tmpCharacter = new cCharacter();
    std::string meshName;
    g_tmpCharacter->LoadCharacterFromAssimp(filename, meshName);

    //int numAnimations = animations.size();
    //for (int i = 0; i < numAnimations; i++)
    //{
    //    // Load animation
    //    g_tmpCharacter->LoadAnimationFromAssimp(animations[i].c_str());
    //}

    g_tmpCharacterGameObject->Renderer.MeshId = gModelVec.size();
    g_tmpCharacterGameObject->meshName = meshName;
    gModelVec.push_back(g_tmpCharacter->GetModel());
    return g_tmpCharacterGameObject;
}

void GDP_LoadAnimation(const char* name, AnimationData animation)
{
    g_AnimationManager.LoadAnimation(name, animation);
}

void GDP_LoadModel(unsigned int& id, const char* filepath, bool withBones) {
    Model* model = nullptr;
    if (withBones)
        model = new Model(filepath, true);
    else
        model = new Model(filepath);
    printf("Loaded model \"%s\" with %d vertices, %d triangles, id is: %d\n", filepath, model->Vertices.size(), model->NumTriangles, model->Vbo);
    id = gModelVec.size();
    gModelVec.push_back(model);
}

void GDP_CreateModel(unsigned int& id, const std::vector<glm::vec3>& Vertices, const std::vector<int>& triangles) {
    Model* model = new Model(Vertices, triangles);
    printf("Created model with %d triangles, id is: %d\n", model->NumTriangles, model->Vbo);
    id = gModelVec.size();
    gModelVec.push_back(model);
}

void GDP_GetModelData(unsigned int id,
    std::vector<glm::vec3>& Vertices, std::vector<int>& triangles,
    unsigned int& numTriangles, unsigned int& vbo) {

    assert(id < gModelVec.size());
    const Model* model = gModelVec[id];
    numTriangles = model->NumTriangles;
    vbo = model->Vbo;
    Vertices = model->Vertices;
    triangles = model->triangles;
}

Model* GetModel(unsigned int id) {
    assert(id < gModelVec.size());
    Model* model = gModelVec[id];
    return model;
}

void GDP_CreateMaterial(unsigned int& id, unsigned int textureId, const glm::vec3& color) {
    Material* material = new Material();
    material->TextureId = textureId;
    material->Color = color;
    id = gMaterialVec.size();
    gMaterialVec.push_back(material);
}