#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    unsigned int ID;
    std::string type;
    std::string path;

    Texture(const char* path, const std::string& type);
    void bind(int unit);
};