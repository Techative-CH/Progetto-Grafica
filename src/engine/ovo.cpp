#include "ovoReader.h"
#include "node.h"

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>

#include <cstdio>
#include <cstring>
#include <iostream>

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

    Node* root = new Node("OVO_ROOT");

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

            position +=
                static_cast<unsigned int>(
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

            position +=
                static_cast<unsigned int>(
                    strlen(targetName)
                    ) + 1;

            std::cout
                << "NODE: "
                << nodeName
                << ", children: "
                << children
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