#pragma once

#include <GL/glew.h>

struct Texture
{
    Texture(GLuint textureId);

    void Use() const;

    GLuint GetGlId() const;

  private:
    const GLuint m_textureId;
};
