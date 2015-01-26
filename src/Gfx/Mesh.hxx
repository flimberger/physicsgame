#pragma once

#include <glm/glm.hpp>

#include <GL/glew.h>

#include <vector>

struct Mesh
{
    Mesh(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvCoords,
         std::vector<glm::vec3> &normals);
    ~Mesh();

    void Draw() const;

    std::size_t GetNumberOfVertices() const;

  private:
    const std::size_t m_numberOfVertices;

    // GLuint m_elementBufferId;
    GLuint m_normalBufferId;
    GLuint m_uvcBufferId;
    GLuint m_vertexArrayId;
    GLuint m_vertexBufferId;
};
