#include "pch.h"
#include "include/opengl/file.h"

namespace util
{
    std::string File::ReadTextFile(const std::string& path) {
        std::ifstream file(path, std::ifstream::in);
        std::stringstream output;
        if (!file.is_open()) throw std::runtime_error("Failed to read file. (" + path + ")");
        output << file.rdbuf();
        return output.str();
    }
}
