#include "Texture.hxx"

Texture::Texture(GLuint textureId) : m_textureId{textureId} {}

void Texture::Use() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}

GLuint Texture::GetGlId() const { return m_textureId; }
