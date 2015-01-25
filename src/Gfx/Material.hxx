#pragma once

#include "Program.hxx"
#include "Texture.hxx"

struct Material
{
    Material(const std::shared_ptr<Program> &shaderProgram,
             const std::shared_ptr<Texture> &texture);

    void Use() const;

  private:
    std::shared_ptr<Program> m_shaderProgram;
    std::shared_ptr<Texture> m_texture;

    GLint m_uniformTextureId;
};
