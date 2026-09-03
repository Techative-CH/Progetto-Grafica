/**
 * @file ovo.cpp
 * @brief OVO reader file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
#include "ovoReader.h"
#include "node.h"
#include "mesh.h"
#include "light.h"
#include "omniLight.h"
#include "directionalLight.h"
#include "spotLight.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <cmath>

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

    void attachNode(Eng::Node* node, unsigned int children, Eng::Node*& root, std::vector<ParentInfo>& parentStack)
    {
        // If parent stack is not empty insert child
        if (!parentStack.empty())
        {
            ParentInfo& parent = parentStack.back();
            parent.node->addChild(node);

            if (parent.remainingChildren > 0)
                parent.remainingChildren--;
        }
        else
        {
            root = node;
        }

        // When parent is complete remove it from parent stack
        while (!parentStack.empty() && parentStack.back().remainingChildren == 0)
        {
            parentStack.pop_back();
        }

        // If the node has children add it to the parent stack
        if (children > 0)
            parentStack.push_back({ node, children });
    }
}

Eng::Node* Eng::OvoReader::load(const std::string& filename)
{
    std::filesystem::path ovoPath(filename);
    std::filesystem::path baseDir = ovoPath.parent_path();

    FILE* dat = nullptr;

#ifdef _WIN32
    fopen_s(&dat, filename.c_str(), "rb"); // Open the file on Windows, where fopen is considered unsafe
#else
    dat = fopen(filename.c_str(), "rb"); // Open the file on non-Windows systems
#endif

    if (dat == nullptr)
    {
        std::cerr << "[OVO ERROR] Unable to open file: " << filename << std::endl;
        return nullptr;
    }

    Node* root = nullptr;
    std::vector<ParentInfo> parentStack;
    std::unordered_map<std::string, Eng::Material*> materials;

    unsigned int chunkId;
    unsigned int chunkSize;

    while (true)
    {
        if (fread(&chunkId, sizeof(unsigned int), 1, dat) != 1)
            break;

        if (fread(&chunkSize, sizeof(unsigned int), 1, dat) != 1)
        {
            delete root;
            fclose(dat);
            return nullptr;
        }

        char* data = new char[chunkSize];

        if (fread(data, sizeof(char), chunkSize, dat) != chunkSize)
        {
            std::cerr << "[OVO ERROR] Unable to read file: " << filename << std::endl;

            delete[] data;
            delete root;
            fclose(dat);

            return nullptr;
        }

        unsigned int position = 0;

        switch (static_cast<OvObject::Type>(chunkId))
        {
        case OvObject::Type::OBJECT:
        {
            unsigned int versionId;
            memcpy(&versionId, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            std::cout << "[OVO] Version " << versionId << std::endl;

            break;
        }

        case OvObject::Type::NODE:
        {
            const char* nodeName = data + position;
            position += static_cast<unsigned int>(strlen(nodeName)) + 1;

            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            position += sizeof(glm::mat4);

            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            const char* targetName = data + position;
            position += static_cast<unsigned int>(strlen(targetName)) + 1;

            Node* node = new Node(nodeName);
            node->setLocalMatrix(matrix);

            attachNode(node, children, root, parentStack);

            std::cout << "[OVO] Node loaded: " << nodeName << " (" << children << " children)" << std::endl;
            
            break;
        }

        case OvObject::Type::LIGHT:
        {
            // Light name
            const char* lightName = data + position;
            position += static_cast<unsigned int>(strlen(lightName)) + 1;

            // Matrix
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            position += sizeof(glm::mat4);

            // Number of children
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            // Optional target
            const char* targetName = data + position;
            position += static_cast<unsigned int>(strlen(targetName)) + 1;

            // Light type
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Color
            glm::vec3 color;
            memcpy(&color, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Radius
            float radius;
            memcpy(&radius, data + position, sizeof(float));
            position += sizeof(float);

            // Direction
            glm::vec3 direction;
            memcpy(&direction, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Cutoff
            float cutoff;
            memcpy(&cutoff, data + position, sizeof(float));
            position += sizeof(float);

            // Spot exponent
            float spotExponent;
            memcpy(&spotExponent, data + position, sizeof(float));
            position += sizeof(float);

            // Cast shadows
            unsigned char castShadows;
            memcpy(&castShadows, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Volumetric
            unsigned char isVolumetric;
            memcpy(&isVolumetric, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Create the corresponding engine light type
            Eng::Light* light = nullptr;

            switch (subtype)
            {
            case 0: // Omnidirectional
            {
                auto* omni = new Eng::OmniLight(lightName, color);
                omni->setRadius(radius);
                light = omni;
                break;
            }

            case 1: // Directional
            {
                auto* directional = new Eng::DirectionalLight(lightName, color);
                directional->setDirection(direction);
                light = directional;
                break;
            }

            case 2: // Spotlight
            {
                auto* spot = new Eng::SpotLight(lightName, color);

                spot->setRadius(radius);
                spot->setDirection(direction);
                spot->setCutoff(cutoff);
                spot->setSpotExponent(spotExponent);

                light = spot;

                break;
            }

            default:
                std::cerr << "[OVO ERROR] Unsupported light type: " << static_cast<int>(subtype) << std::endl;
                break;
            }

            if (light == nullptr)
                break;

            light->setLocalMatrix(matrix);

            light->setCastShadows(castShadows != 0);
            light->setVolumetric(isVolumetric != 0);

            attachNode(light, children, root, parentStack);

            std::cout << "[OVO] Light loaded: " << lightName << " | type: " << static_cast<int>(subtype) << std::endl;

            break;
        }

        case OvObject::Type::MESH:
        {
            // Mesh name
            const char* meshName = data + position;
            position += static_cast<unsigned int>(strlen(meshName)) + 1;

            // Mesh matrix
            glm::mat4 matrix;
            memcpy(&matrix, data + position, sizeof(glm::mat4));
            position += sizeof(glm::mat4);

            // Mesh number of children nodes
            unsigned int children;
            memcpy(&children, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            // Optional target node
            const char* targetName = data + position;
            position += static_cast<unsigned int>(strlen(targetName)) + 1;

            // Mesh subtype
            unsigned char subtype;
            memcpy(&subtype, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            // Material name
            const char* materialName = data + position;
            position += static_cast<unsigned int>(strlen(materialName)) + 1;

            // Radius
            float radius;
            memcpy(&radius, data + position, sizeof(float));
            position += sizeof(float);

            // Bounding box minimum
            glm::vec3 bBoxMin;
            memcpy(&bBoxMin, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Bounding box maximum
            glm::vec3 bBoxMax;
            memcpy(&bBoxMax, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Optional physics properties
            unsigned char hasPhysics;
            memcpy(&hasPhysics, data + position, sizeof(unsigned char));
            position += sizeof(unsigned char);

            if (hasPhysics)
            {
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
                memcpy(&mp, data + position, sizeof(PhysProps));
                position += sizeof(PhysProps);

                // Custom hulls
                if (mp.nrOfHulls)
                {
                    for (unsigned int h = 0; h < mp.nrOfHulls; h++)
                    {
                        // Hull number of vertices
                        unsigned int nrOfVertices;
                        memcpy(&nrOfVertices, data + position, sizeof(unsigned int));
                        position += sizeof(unsigned int);

                        // Hull number of faces
                        unsigned int nrOfFaces;
                        memcpy(&nrOfFaces, data + position, sizeof(unsigned int));
                        position += sizeof(unsigned int);

                        // Hull centroid
                        glm::vec3 centroid;
                        memcpy(&centroid, data + position, sizeof(glm::vec3));
                        position += sizeof(glm::vec3);

                        // Hull vertices
                        for (unsigned int v = 0; v < nrOfVertices; v++)
                        {
                            glm::vec3 vertex;
                            memcpy(&vertex, data + position, sizeof(glm::vec3));
                            position += sizeof(glm::vec3);
                        }

                        // Hull faces
                        for (unsigned int f = 0; f < nrOfFaces; f++)
                        {
                            unsigned int face[3];
                            memcpy(face, data + position, sizeof(unsigned int) * 3);
                            position += sizeof(unsigned int) * 3;
                        }
                    }
                }
            }

            // Number of lods
            unsigned int LODs;
            memcpy(&LODs, data + position, sizeof(unsigned int));
            position += sizeof(unsigned int);

            // Engine vertex
            std::vector<Eng::Vertex> meshVertices;
            std::vector<unsigned int> meshIndices;

            for (unsigned int l = 0; l < LODs; l++)
            {
                // Number of vertices
                unsigned int vertices;
                memcpy(&vertices, data + position, sizeof(unsigned int));
                position += sizeof(unsigned int);

                // Number of faces
                unsigned int faces;
                memcpy(&faces, data + position, sizeof(unsigned int));
                position += sizeof(unsigned int);

                // Vertices
                for (unsigned int v = 0; v < vertices; v++)
                {
                    // Vertex coordinates
                    glm::vec3 vertexPosition;
                    memcpy(&vertexPosition, data + position, sizeof(glm::vec3));
                    position += sizeof(glm::vec3);

                    // Vertex normal
                    unsigned int normalData;
                    memcpy(&normalData, data + position, sizeof(unsigned int));
                    position += sizeof(unsigned int);

                    glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);

                    // Texture coordinates
                    unsigned int textureData;
                    memcpy(&textureData, data + position, sizeof(unsigned int));
                    position += sizeof(unsigned int);

                    glm::vec2 uv = glm::unpackHalf2x16(textureData);

                    // Tangent
                    unsigned int tangentData;
                    memcpy(&tangentData, data + position, sizeof(unsigned int));
                    position += sizeof(unsigned int);

                    glm::vec4 tangent = glm::unpackSnorm3x10_1x2(tangentData);

                    // Store vertex data only for the first LOD
                    if (l == 0)
                    {
                        Eng::Vertex vertex;

                        vertex.position = vertexPosition;
                        vertex.normal = glm::vec3(normal);
                        vertex.texCoord = uv;

                        meshVertices.push_back(vertex);
                    }
                }

                // Faces
                for (unsigned int f = 0; f < faces; f++)
                {
                    unsigned int face[3];
                    memcpy(face, data + position, sizeof(unsigned int) * 3);
                    position += sizeof(unsigned int) * 3;

                    if (l == 0)
                    {
                        meshIndices.push_back(face[0]);
                        meshIndices.push_back(face[1]);
                        meshIndices.push_back(face[2]);
                    }
                }
            }

            // Create the engine mesh with the loaded data
            Eng::Mesh* mesh = new Eng::Mesh(meshName);

            mesh->setLocalMatrix(matrix);
            mesh->setVertices(meshVertices);
            mesh->setIndices(meshIndices);

            // Assign the corresponding material to the mesh
            auto materialIt = materials.find(materialName);
            if (materialIt != materials.end())
                mesh->setMaterial(materialIt->second);

            attachNode(mesh, children, root, parentStack);

            // Insert the mesh into the scene graph
            std::cout << "[OVO] Mesh loaded: " << meshName
                << " | vertices: " << meshVertices.size()
                << " | triangles: " << meshIndices.size() / 3
                << std::endl;

            break;
        }

        case OvObject::Type::MATERIAL:
        {
            // Name
            const char* materialName = data + position;
            position += static_cast<unsigned int>(strlen(materialName)) + 1;

            // Emissive component
            glm::vec3 emission;
            memcpy(&emission, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Albedo
            glm::vec3 albedo;
            memcpy(&albedo, data + position, sizeof(glm::vec3));
            position += sizeof(glm::vec3);

            // Roughness
            float roughness;
            memcpy(&roughness, data + position, sizeof(float));
            position += sizeof(float);

            // Metalness
            float metalness;
            memcpy(&metalness, data + position, sizeof(float));
            position += sizeof(float);

            // Alpha
            float alpha;
            memcpy(&alpha, data + position, sizeof(float));
            position += sizeof(float);

            // Texture name
            const char* textureName = data + position;
            position += static_cast<unsigned int>(strlen(textureName)) + 1;

            // Maps
            const char* normalMapName = data + position;
            position += static_cast<unsigned int>(strlen(normalMapName)) + 1;

            const char* heightMapName = data + position;
            position += static_cast<unsigned int>(strlen(heightMapName)) + 1;

            const char* roughnessMapName = data + position;
            position += static_cast<unsigned int>(strlen(roughnessMapName)) + 1;

            const char* metalnessMapName = data + position;
            position += static_cast<unsigned int>(strlen(metalnessMapName)) + 1;

            // Convert OVO material properties to the Blinn-Phong lighting model
            Eng::Material* material = new Eng::Material(materialName);

            glm::vec3 ambient = albedo * 0.2f;
            material->setAmbient(ambient.r, ambient.g, ambient.b, alpha);

            glm::vec3 diffuse = albedo * 0.6f;
            material->setDiffuse(diffuse.r, diffuse.g, diffuse.b, alpha);

            glm::vec3 specular = albedo * 0.4f;
            material->setSpecular(specular.r, specular.g, specular.b, alpha);

            material->setEmission(emission.r, emission.g, emission.b, alpha);

            float shininess = (1.0f - std::sqrt(roughness)) * 128.0f;
            material->setShininess(shininess);


            // Load texture from file and apply it to material
            if (std::strcmp(textureName, "[none]") != 0)
            {
                std::filesystem::path texturePath = baseDir / textureName;

                Eng::Texture* texture = new Eng::Texture(textureName);

                texture->setFilter(Eng::TextureFilter::LINEAR);
                texture->setWrap(Eng::TextureWrap::REPEAT);

                if (texture->loadFromFile(texturePath.string()))
                {
                    material->setTexture(texture);
                    std::cout << "[OVO] Texture loaded: " << textureName << std::endl;
                }
                else
                {
                    delete texture;
                    std::cerr << "[OVO ERROR] Unable to load texture: " << texturePath.string() << std::endl;
                }
            }

            materials[materialName] = material;

            std::cout << "[OVO] Material loaded: " << materialName << std::endl;

            break;
        }

        default:
            break;
        }

        delete[] data;
    }

    fclose(dat);

    return root;
}