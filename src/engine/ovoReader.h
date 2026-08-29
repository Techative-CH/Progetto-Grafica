// ovoReader.h
#pragma once

#include "node.h"

#include <string>

namespace Eng
{
    class OvoReader
    {
    public:
        OvoReader() = default;
        ~OvoReader() = default;

        Node* load(const std::string& filename);
    };
}