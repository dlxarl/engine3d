#include "Plane.h"

Plane::Plane() {
    float vertices[] = {
        5.0f, 0.0f,  5.0f,    0.0f, 1.0f, 0.0f,   10.0f, 0.0f,
       -5.0f, 0.0f,  5.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
       -5.0f, 0.0f, -5.0f,    0.0f, 1.0f, 0.0f,    0.0f, 10.0f,

        5.0f, 0.0f,  5.0f,    0.0f, 1.0f, 0.0f,   10.0f, 0.0f,
       -5.0f, 0.0f, -5.0f,    0.0f, 1.0f, 0.0f,    0.0f, 10.0f,
        5.0f, 0.0f, -5.0f,    0.0f, 1.0f, 0.0f,   10.0f, 10.0f
   };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Plane::draw(Shader& shader) {
    shader.setMat4("model", model);

    shader.setBool("material.hasAlbedo", false);
    shader.setBool("material.hasNormal", false);
    shader.setBool("material.hasMetallic", false);
    shader.setBool("material.hasRoughness", false);
    shader.setBool("material.hasAO", false);

    for(unsigned int i = 0; i < textures.size(); i++) {
        std::string name = textures[i]->type;

        if(name == "texture_albedo") {
            shader.setInt("material.albedoMap", i);
            shader.setBool("material.hasAlbedo", true);
        } else if(name == "texture_normal") {
            shader.setInt("material.normalMap", i);
            shader.setBool("material.hasNormal", true);
        } else if(name == "texture_metallic") {
            shader.setInt("material.metallicMap", i);
            shader.setBool("material.hasMetallic", true);
        } else if(name == "texture_roughness") {
            shader.setInt("material.roughnessMap", i);
            shader.setBool("material.hasRoughness", true);
        } else if(name == "texture_ao") {
            shader.setInt("material.aoMap", i);
            shader.setBool("material.hasAO", true);
        }

        textures[i]->bind(i);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}