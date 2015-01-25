#include "Material.hxx"

#include "Program.hxx"

Material::Material(const std::shared_ptr<Program> &shaderProgram,
                   const std::shared_ptr<Texture> &texture)
    : m_shaderProgram{shaderProgram}, m_texture{texture},
      m_uniformTextureId{
          glGetUniformLocation(shaderProgram->GetGlId(), "myTextureSampler")}
{
}

void Material::Use() const
{
    m_texture->Use();
    glUniform1i(m_uniformTextureId, 0);
}
