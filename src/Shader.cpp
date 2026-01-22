#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#if __has_include(<filesystem>)
  #include <filesystem>
#endif

static std::string ReadTextFileOrThrow(const char* path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
#if __has_include(<filesystem>)
        std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << path
                  << " | CWD=" << std::filesystem::current_path() << "\n";
#else
        std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << path << "\n";
#endif
        throw std::runtime_error(std::string("Cannot open shader file: ") + path);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    if (content.empty())
    {
        std::cerr << "ERROR::SHADER::FILE_EMPTY: " << path << "\n";
        throw std::runtime_error(std::string("Shader file is empty: ") + path);
    }

    return content;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    ID = 0;

    std::string vertexCode;
    std::string fragmentCode;

    try
    {
        vertexCode   = ReadTextFileOrThrow(vertexPath);
        fragmentCode = ReadTextFileOrThrow(fragmentPath);
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR::SHADER::FILE_READ_FAILED: " << e.what() << "\n";
        // НІЧОГО не компілюємо, інакше можна зловити crash у драйвері
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use()
{
    // якщо шейдер не створився — не падаємо
    if (ID == 0) return;
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    if (ID == 0) return;
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    if (ID == 0) return;
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    if (ID == 0) return;
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    if (ID == 0) return;
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    if (ID == 0) return;
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success = 0;
    char infoLog[1024];

    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
                      << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
                      << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- \n";
        }
    }
}
