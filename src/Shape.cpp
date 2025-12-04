#include "Shape.h"

Shape::Shape() {
    model = glm::mat4(1.0f);
    color = glm::vec3(1.0f);
    VAO = 0;
    VBO = 0;
}

Shape::~Shape() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}

void Shape::setPosition(glm::vec3 pos) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
}

void Shape::rotate(float angle, glm::vec3 axis) {
    model = glm::rotate(model, glm::radians(angle), axis);
}

void Shape::setScale(glm::vec3 scaleVec) {
    model = glm::scale(model, scaleVec);
}

void Shape::setColor(glm::vec3 newColor) {
    color = newColor;
}

glm::vec3 Shape::getColor() const {
    return color;
}

void Shape::addTexture(std::shared_ptr<Texture> tex) {
    textures.push_back(tex);
}