#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <vector>

class Mesh
{
public:
    // be careful with copying and stuff!
    Mesh();
    Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &uvCoords,
         const std::vector<glm::vec3> &normals);
    ~Mesh();

    const std::vector<glm::vec3> &vertices() const;
    const std::vector<glm::vec2> &uvCoords() const;
    const std::vector<glm::vec3> &normals() const;

    //! Prepare for use, i.e. prepare OpenGL stuff
    void setup();

    void draw();

private:
    void teardown();

    bool m_active;

    GLuint m_vbo; // vertex buffer id
    GLuint m_tbo; // texture (uv coordinates) buffer id
    GLuint m_nbo; // normal buffer id

    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvCoords;
    std::vector<glm::vec3> m_normals;
};
