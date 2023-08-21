#pragma once

#include <fstream>
#include <sstream>

#include "axgl/namespace.h"


NAMESPACE_AXGL

inline std::string read_text_file(const std::string& path)
{
    std::ifstream file(path, std::ifstream::in);
    std::stringstream output;

    if (!file.is_open())
        SPDLOG_ERROR("Failed to read file: {}", path);

    output << file.rdbuf();
    return output.str();
}

NAMESPACE_AXGL_END
