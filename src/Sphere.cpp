#include "Sphere.h"
#include <vector>
#include <cmath>

const float PI = 3.14159265359f;

Sphere::Sphere(float radius, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(z);
            vertices.push_back(y);

            nx = x * lengthInv;
            ny = z * lengthInv;
            nz = y * lengthInv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

            // 3. TexCoords (s, t)
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    for(int i = 0; i < stackCount; ++i)
    {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Sphere::draw(Shader& shader) {
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
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}