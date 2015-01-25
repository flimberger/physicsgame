#include "Shader.hxx"

#include <cstdio>
#include <iostream>
#include <vector>

static const std::size_t BUFFER_SIZE = 2048u;

Shader::Shader(shader_t type, const std::string &assetPath)
    : m_sourceFilePath{assetPath}, m_shaderType{type}, m_compiled{false}
{
    GLuint shaderType;

    switch (type) {
    case VERTEX_SHADER:
        shaderType = GL_VERTEX_SHADER;
        break;
    case FRAGMENT_SHADER:
        shaderType = GL_FRAGMENT_SHADER;
        break;
    }
    m_shaderId = glCreateShader(shaderType);
}

Shader::~Shader() { glDeleteShader(m_shaderId); }

bool Shader::Compile()
{
    char sourceBuffer[BUFFER_SIZE];
    std::string sourceText;
    FILE *fp = std::fopen(m_sourceFilePath.c_str(), "r");
    size_t nChars;

    if (fp == nullptr) {
        return false;
    }
    while ((nChars = std::fread(sourceBuffer, sizeof(*sourceBuffer),
                                BUFFER_SIZE - 1, fp)) > 0) {
        sourceBuffer[nChars] = '\0';
        sourceText.append(sourceBuffer);
    }
    if (std::ferror(fp)) {
        std::fclose(fp);

        return false;
    }
    std::fclose(fp);

    GLint result = GL_FALSE;
    const char *sourceTextPtr = sourceText.c_str();

    glShaderSource(m_shaderId, 1, &sourceTextPtr, nullptr);
    glCompileShader(m_shaderId);
    glGetShaderiv(m_shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint logSize;

        glGetShaderiv(m_shaderId, GL_INFO_LOG_LENGTH, &logSize);

        std::vector<char> infoLog(
            static_cast<std::vector<char>::size_type>(logSize));

        glGetShaderInfoLog(m_shaderId, logSize, nullptr, &infoLog[0]);
        std::cerr << m_sourceFilePath << ": " << &infoLog[0] << std::endl;

        std::cerr << sourceText << std::endl;

        return false;
    }
    m_compiled = true;

    return true;
}

GLuint Shader::GetGlId() const { return m_shaderId; }

shader_t Shader::GetType() const { return m_shaderType; }

bool Shader::IsCompiled() const { return m_compiled; }
