#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Shader.h"
#include "Texture.h"

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
    void addTexture(std::shared_ptr<Texture> tex);

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 scale;

    bool useGravity;
    bool isStatic;
    bool hasCollision;

    bool checkCollision(Shape& other);

protected:
    unsigned int VAO, VBO;
    glm::mat4 model;
    glm::vec3 color;
    std::vector<std::shared_ptr<Texture>> textures;

    void updateModelMatrix();
};