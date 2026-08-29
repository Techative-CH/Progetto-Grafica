#include "ovoReader.h"
#include "node.h"
#include "mesh.h"
#include "light.h"

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>

namespace
{
    class OvObject
    {
    public:
        enum class Type : int
        {
            OBJECT = 0,
            NODE,
            OBJECT2D,
            OBJECT3D,
            LIST,

            BUFFER,
            SHADER,
            TEXTURE,
            FILTER,
            MATERIAL,
            FBO,
            QUAD,
            BOX,
            SKYBOX,
            FONT,
            CAMERA,
            LIGHT,
            BONE,
            MESH,
            SKINNED,
            INSTANCED,
            PIPELINE,
            EMITTER,

            ANIM,
            PHYSICS,

            LAST
        };
    };

    class OvMesh
    {
    public:
        enum class Subtype : int
        {
            DEFAULT = 0,
            NORMALMAPPED,
            TESSELLATED,
            LAST
        };
    };

    class OvLight
    {
    public:
        enum class Subtype : int
        {
            OMNI = 0,
            DIRECTIONAL,
            SPOT,
            LAST
        };
    };

    struct ParentInfo
    {
        Eng::Node* node;
        unsigned int remainingChildren;
    };

    void attachNode(
        Eng::Node* node,
        unsigned int children,
        Eng::Node*& root,
        std::vector<ParentInfo>& parentStack)
    {
        if (!parentStack.empty())
        {
            parentStack.back().node->addChild(node);

            if (parentStack.back().remainingChildren > 0)
                parentStack.back().remainingChildren--;
        }
        else
        {
            root = node;
        }

        while (!parentStack.empty() &&
            parentStack.back().remainingChildren == 0)
        {
            parentStack.pop_back();
        }

        if (children > 0)
        {
            parentStack.push_back(
                { node, children }
            );
        }
    }
}

Eng::Node* Eng::OvoReader::load(const std::string& filename)
{
    FILE* dat = nullptr;

#ifdef _WIN32
    fopen_s(&dat, filename.c_str(), "rb");
#else
    dat = fopen(filename.c_str(), "rb");
#endif

    if (dat == nullptr)
    {
        std::cerr
            << "Unable to open OVO file: "
            << filename
            << std::endl;

        return nullptr;
    }

    Node* root = nullptr;
    std::vector<ParentInfo> parentStack;
    std::unordered_map<std::string, Eng::Material*> materials;

    unsigned int chunkId;
    unsigned int chunkSize;

    while (true)
    {
        if (fread(
            &chunkId,
            sizeof(unsigned int),
            1,
            dat
        ) != 1)
        {
            break;
        }

        if (fread(
            &chunkSize,
            sizeof(unsigned int),
            1,
            dat
        ) != 1)
        {
            delete root;
            fclose(dat);
            return nullptr;
        }

        char* data = new char[chunkSize];

        if (fread(
            data,
            sizeof(char),
            chunkSize,
            dat
        ) != chunkSize)
        {
            std::cerr
                << "Unable to read OVO file: "
                << filename
                << std::endl;

            delete[] data;
            delete root;
            fclose(dat);

            return nullptr;
        }

        unsigned int position = 0;

        std::cout
            << "Chunk ID: "
            << chunkId
            << ", size: "
            << chunkSize
            << std::endl;

        switch (static_cast<OvObject::Type>(chunkId))
        {
        case OvObject::Type::OBJECT:
        {
            unsigned int versionId;

            memcpy(
                &versionId,
                data + position,
                sizeof(unsigned int)
            );

            position += sizeof(unsigned int);

            std::cout
                << "OVO version: "
                << versionId
                << std::endl;
        }
        break;

        case OvObject::Type::NODE:
        {
            const char* nodeName = data + position;

            position += static_cast<unsigned int>(
                strlen(nodeName)
                ) + 1;

            glm::mat4 matrix;

            memcpy(
                &matrix,
                data + position,
                sizeof(glm::mat4)
            );

            position += sizeof(glm::mat4);

            unsigned int children;

            memcpy(
                &children,
                data + position,
                sizeof(unsigned int)
            );

            position += sizeof(unsigned int);

            const char* targetName = data + position;

            position += static_cast<unsigned int>(
                strlen(targetName)
                ) + 1;

            Node* node = new Node(nodeName);
            node->setLocalMatrix(matrix);

            attachNode(
                node,
                children,
                root,
                parentStack
            );

            std::cout
                << "NODE: "
                << nodeName
                << ", children: "
                << children
                << std::endl;
        }
        break;

        case OvObject::Type::LIGHT:
        {
            // Light name:
            const char* lightName = data + position;
            position += static_cast<unsigned int>(strlen(lightName)) + 1;

            // Matrix:
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            position += sizeof(glm::mat4);

            // Nr. children:
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            // Optional target:
            const char* targetName = data + position;
            position += static_cast<unsigned int>(strlen(targetName)) + 1;

            // Light subtype:
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Color:
            glm::vec3 color;
            memcpy(&color, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Radius:
            float radius;
            memcpy(&radius, data + position, sizeof(float));
            position += sizeof(float);

            // Direction:
            glm::vec3 direction;
            memcpy(&direction, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Cutoff:
            float cutoff;
            memcpy(&cutoff, data + position, sizeof(float));
            position += sizeof(float);

            // Spot exponent:
            float spotExponent;
            memcpy(&spotExponent, data + position, sizeof(float));
            position += sizeof(float);

            // Cast shadows:
            unsigned char castShadows;
            memcpy(&castShadows, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Volumetric:
            unsigned char isVolumetric;
            memcpy(&isVolumetric, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);


            // -----------------------------------------
            // OUR ENGINE ADAPTATION
            // -----------------------------------------

            Eng::Light* light = new Eng::Light(lightName);

            light->setLocalMatrix(matrix);

            light->setSubtype(
                static_cast<Eng::Light::Subtype>(subtype)
            );

            light->setColor(
                color.r,
                color.g,
                color.b
            );

            light->setRadius(radius);
            light->setDirection(direction);
            light->setCutoff(cutoff);
            light->setSpotExponent(spotExponent);
            light->setCastShadows(castShadows != 0);
            light->setVolumetric(isVolumetric != 0);

            attachNode(
                light,
                children,
                root,
                parentStack
            );

            std::cout
                << "LIGHT: "
                << lightName
                << ", subtype: "
                << static_cast<int>(subtype)
                << ", color: "
                << color.r << ", "
                << color.g << ", "
                << color.b
                << ", target: "
                << targetName
                << std::endl;
        }
        break;

        case OvObject::Type::MESH:
        {
            // Mesh name:
            const char* meshName = data + position;
            position += static_cast<unsigned int>(strlen(meshName)) + 1;

            // Mesh matrix:
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            position += sizeof(glm::mat4);

            // Mesh nr. of children nodes:
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            // Optional target node:
            const char* targetName = data + position;
            position += static_cast<unsigned int>(strlen(targetName)) + 1;

            // Mesh subtype:
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Material name:
            const char* materialName = data + position;
            position += static_cast<unsigned int>(strlen(materialName)) + 1;

            // Bounding sphere radius:
            float radius;
            memcpy(&radius, data + position, sizeof(float));
            position += sizeof(float);

            // Bounding box minimum:
            glm::vec3 bBoxMin;
            memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Bounding box maximum:
            glm::vec3 bBoxMax;
            memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Optional physics properties:
            unsigned char hasPhysics;
            memcpy(&hasPhysics, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            if (hasPhysics)
            {
                // Same structure used by the professor's OVO Reader.
                struct PhysProps
                {
                    unsigned char type;
                    unsigned char contCollisionDetection;
                    unsigned char collideWithRBodies;
                    unsigned char hullType;

                    glm::vec3 massCenter;

                    float mass;
                    float staticFriction;
                    float dynamicFriction;
                    float bounciness;
                    float linearDamping;
                    float angularDamping;

                    unsigned int nrOfHulls;
                    unsigned int _pad;

                    void* physObj;
                    void* hull;
                };

                PhysProps mp;

                memcpy(
                    &mp,
                    data + position,
                    sizeof(PhysProps)
                );

                position += sizeof(PhysProps);

                // Custom hull(s):
                if (mp.nrOfHulls)
                {
                    for (unsigned int h = 0; h < mp.nrOfHulls; h++)
                    {
                        // Hull number of vertices:
                        unsigned int nrOfVertices;

                        memcpy(
                            &nrOfVertices,
                            data + position,
                            sizeof(unsigned int)
                        );

                        position += sizeof(unsigned int);

                        // Hull number of faces:
                        unsigned int nrOfFaces;

                        memcpy(
                            &nrOfFaces,
                            data + position,
                            sizeof(unsigned int)
                        );

                        position += sizeof(unsigned int);

                        // Hull centroid:
                        glm::vec3 centroid;

                        memcpy(
                            &centroid,
                            data + position,
                            sizeof(glm::vec3)
                        );

                        position += sizeof(glm::vec3);

                        // Hull vertices:
                        for (unsigned int v = 0; v < nrOfVertices; v++)
                        {
                            glm::vec3 vertex;

                            memcpy(
                                &vertex,
                                data + position,
                                sizeof(glm::vec3)
                            );

                            position += sizeof(glm::vec3);
                        }

                        // Hull faces:
                        for (unsigned int f = 0; f < nrOfFaces; f++)
                        {
                            unsigned int face[3];

                            memcpy(
                                face,
                                data + position,
                                sizeof(unsigned int) * 3
                            );

                            position += sizeof(unsigned int) * 3;
                        }
                    }
                }
            }

            // Nr. of LODs:
            unsigned int LODs;

            memcpy(
                &LODs,
                data + position,
                sizeof(unsigned int)
            );

            position += sizeof(unsigned int);

            // OUR ENGINE DATA:
            std::vector<Eng::Vertex> meshVertices;
            std::vector<unsigned int> meshIndices;

            // For each LOD:
            for (unsigned int l = 0; l < LODs; l++)
            {
                // Nr. vertices:
                unsigned int vertices;

                memcpy(
                    &vertices,
                    data + position,
                    sizeof(unsigned int)
                );

                position += sizeof(unsigned int);

                // Nr. faces:
                unsigned int faces;

                memcpy(
                    &faces,
                    data + position,
                    sizeof(unsigned int)
                );

                position += sizeof(unsigned int);

                // Vertices:
                for (unsigned int v = 0; v < vertices; v++)
                {
                    // Vertex coords:
                    glm::vec3 vertexPosition;

                    memcpy(
                        &vertexPosition,
                        data + position,
                        sizeof(glm::vec3)
                    );

                    position += sizeof(glm::vec3);

                    // Vertex normal:
                    unsigned int normalData;

                    memcpy(
                        &normalData,
                        data + position,
                        sizeof(unsigned int)
                    );

                    position += sizeof(unsigned int);

                    glm::vec4 normal =
                        glm::unpackSnorm3x10_1x2(normalData);

                    // Texture coordinates:
                    unsigned int textureData;

                    memcpy(
                        &textureData,
                        data + position,
                        sizeof(unsigned int)
                    );

                    position += sizeof(unsigned int);

                    glm::vec2 uv =
                        glm::unpackHalf2x16(textureData);

                    // Tangent:
                    unsigned int tangentData;

                    memcpy(
                        &tangentData,
                        data + position,
                        sizeof(unsigned int)
                    );

                    position += sizeof(unsigned int);

                    glm::vec4 tangent =
                        glm::unpackSnorm3x10_1x2(tangentData);

                    // OUR ENGINE:
                    // For now we use the first LOD.
                    if (l == 0)
                    {
                        Eng::Vertex vertex;

                        vertex.position = vertexPosition;
                        vertex.normal = glm::vec3(normal);
                        vertex.texCoord = uv;

                        meshVertices.push_back(vertex);
                    }
                }

                // Faces:
                for (unsigned int f = 0; f < faces; f++)
                {
                    unsigned int face[3];

                    memcpy(
                        face,
                        data + position,
                        sizeof(unsigned int) * 3
                    );

                    position += sizeof(unsigned int) * 3;

                    // OUR ENGINE:
                    if (l == 0)
                    {
                        meshIndices.push_back(face[0]);
                        meshIndices.push_back(face[1]);
                        meshIndices.push_back(face[2]);
                    }
                }
            }

            // OUR ENGINE:
            Eng::Mesh* mesh = new Eng::Mesh(meshName);

            mesh->setLocalMatrix(matrix);
            mesh->setVertices(meshVertices);
            mesh->setIndices(meshIndices);

            auto materialIt = materials.find(materialName);

            if (materialIt != materials.end())
            {
                mesh->setMaterial(materialIt->second);
            }

            attachNode(
                mesh,
                children,
                root,
                parentStack
            );

            std::cout
                << "MESH: "
                << meshName
                << ", vertices: "
                << meshVertices.size()
                << ", triangles: "
                << meshIndices.size() / 3
                << ", material: "
                << materialName
                << std::endl;
        }
        break;

        case OvObject::Type::MATERIAL:
        {
            const char* materialName = data + position;
            position += static_cast<unsigned int>(strlen(materialName)) + 1;

            glm::vec3 emission;
            memcpy(&emission, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            glm::vec3 albedo;
            memcpy(&albedo, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            float roughness;
            memcpy(&roughness, data + position, sizeof(float));
            position += sizeof(float);

            float metalness;
            memcpy(&metalness, data + position, sizeof(float));
            position += sizeof(float);

            float alpha;
            memcpy(&alpha, data + position, sizeof(float));
            position += sizeof(float);

            const char* textureName = data + position;
            position += static_cast<unsigned int>(strlen(textureName)) + 1;

            const char* normalMapName = data + position;
            position += static_cast<unsigned int>(strlen(normalMapName)) + 1;

            const char* heightMapName = data + position;
            position += static_cast<unsigned int>(strlen(heightMapName)) + 1;

            const char* roughnessMapName = data + position;
            position += static_cast<unsigned int>(strlen(roughnessMapName)) + 1;

            const char* metalnessMapName = data + position;
            position += static_cast<unsigned int>(strlen(metalnessMapName)) + 1;

            Eng::Material* material = new Eng::Material(materialName);

            material->setEmission(
                emission.r,
                emission.g,
                emission.b
            );

            material->setAmbient(
                albedo.r * 0.2f,
                albedo.g * 0.2f,
                albedo.b * 0.2f
            );

            material->setDiffuse(
                albedo.r,
                albedo.g,
                albedo.b
            );

            glm::vec3 specular =
                glm::mix(
                    glm::vec3(0.04f),
                    albedo,
                    metalness
                );

            material->setSpecular(
                specular.r,
                specular.g,
                specular.b
            );

            material->setShininess(
                (1.0f - roughness) * 128.0f
            );

            materials[materialName] = material;

            std::cout
                << "MATERIAL: "
                << materialName
                << ", albedo: "
                << albedo.r << ", "
                << albedo.g << ", "
                << albedo.b
                << std::endl;
        }
        break;

        default:
            break;
        }

        delete[] data;
    }

    fclose(dat);

    return root;
}