#include "Program.hxx"

#include "Shader.hxx"

#include <iostream>

Program::Program(const std::vector<std::shared_ptr<Shader>> &shaders)
    : m_shaders{shaders}, m_programId{glCreateProgram()}, m_linked{false}
{
}

Program::~Program() { glDeleteProgram(m_programId); }

bool Program::Link()
{
    for (auto &shader : m_shaders) {
        if (!shader->IsCompiled()) {
            if (!shader->Compile()) {
                return false;
            }
        }
        glAttachShader(m_programId, shader->GetGlId());
    }

    GLint result = GL_FALSE;

    glLinkProgram(m_programId);
    glGetProgramiv(m_programId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        GLint logSize;

        glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &logSize);

        std::vector<char> infoLog(
            static_cast<std::vector<char>::size_type>(logSize));

        glGetProgramInfoLog(m_programId, logSize, nullptr, &infoLog[0]);
        std::cerr << &infoLog[0] << std::endl;

        return false;
    }
    m_linked = true;

    return true;
}

void Program::Use() const { glUseProgram(m_programId); }

GLuint Program::GetGlId() const { return m_programId; }
