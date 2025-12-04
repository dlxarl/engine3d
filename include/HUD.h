#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Shader.h"
#include "Texture.h"

class HUD {
public:
    HUD(int width, int height);
    ~HUD();

    void drawSprite(std::shared_ptr<Texture> texture, glm::vec2 position, glm::vec2 size, float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void resize(int width, int height);

private:
    std::unique_ptr<Shader> shader;
    unsigned int VAO;
    unsigned int VBO;
    glm::mat4 projection;
};