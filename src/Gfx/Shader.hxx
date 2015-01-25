#pragma once

#include <GL/glew.h>

#include <string>

enum shader_t { VERTEX_SHADER, FRAGMENT_SHADER };

struct Shader
{
    explicit Shader(shader_t type, const std::string &assetPath);
    ~Shader();

    bool Compile();

    GLuint GetGlId() const;
    shader_t GetType() const;
    bool IsCompiled() const;

  private:
    std::string m_sourceFilePath;
    GLuint m_shaderId;
    const shader_t m_shaderType;
    bool m_compiled;
};
