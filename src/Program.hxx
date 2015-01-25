#pragma once

#include "Shader.hxx"

#include <memory>
#include <vector>

struct Program
{
    Program(const std::vector<std::shared_ptr<Shader>> &shaders);
    ~Program();

    bool Link();
    void Use() const;

    GLuint GetGlId() const;

  private:
    std::vector<std::shared_ptr<Shader>> m_shaders;
    GLuint m_programId;
    bool m_linked;
};
