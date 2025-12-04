#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include <memory>

class PostProcessor {
public:
    PostProcessor(int width, int height);
    ~PostProcessor();

    void beginRender();
    void endRender();
    void draw(const glm::mat4& view, const glm::mat4& projection, float currentFPS);
    void resize(int width, int height);

    bool enabled = true;

private:
    unsigned int FBO;
    unsigned int colorTexture;
    unsigned int depthTexture;
    unsigned int VAO, VBO;
    std::unique_ptr<Shader> shader;
    
    glm::mat4 prevViewProjection;
    bool firstFrame;

    void initRenderData();
};