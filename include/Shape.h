#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Shape {
public:
    Shape();
    virtual ~Shape();
    virtual void draw(Shader& shader) = 0;

    void setPosition(glm::vec3 pos);
    void rotate(float angle, glm::vec3 axis);
    void setScale(glm::vec3 scaleVec);

    void setColor(glm::vec3 newColor);
    glm::vec3 getColor() const;

protected:
    unsigned int VAO, VBO;
    glm::mat4 model;
    glm::vec3 color;
};