#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    unsigned int ID;
    std::string type;

    Texture(const char* path);
    void bind();
};