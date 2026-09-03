/**
 * @file ovoReader.h
 * @brief OVO reader include file
 *
 * @author Samuel Banfi (C) SUPSI [samuel.banfi@supsi.ch]
 */
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