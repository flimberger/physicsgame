#include "Mesh.hpp"

#include <iostream>

Mesh::Mesh()
  : m_active { false }
{}

Mesh::Mesh(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec2> &uvCoords,
		   const std::vector<glm::vec3> &normals)
  : m_active { false },
    m_vertices { vertices },
    m_uvCoords { uvCoords },
    m_normals { normals }
{}

Mesh::~Mesh()
{
    teardown();
}

const std::vector<glm::vec3> &Mesh::vertices() const
{
    return m_vertices;
}

const std::vector<glm::vec2> &Mesh::uvCoords() const
{
    return m_uvCoords;
}

const std::vector<glm::vec3> &Mesh::normals() const
{
    return m_normals;
}

void Mesh::setup()
{
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(decltype(m_vertices)::value_type),
                 m_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_tbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
    glBufferData(GL_ARRAY_BUFFER, m_uvCoords.size() * sizeof(decltype(m_uvCoords)::value_type),
                 m_uvCoords.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_nbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(decltype(m_normals)::value_type),
                 m_normals.data(), GL_STATIC_DRAW);

    m_active = true;
}

void Mesh::beginDraw()
{
    if (!m_active) {
        std::cerr << std::endl << "warning, attempted to draw inactive mesh" << std::endl;
        return;
    }

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_tbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_nbo);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void Mesh::endDraw()
{
    if (!m_active) {
        return;
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Mesh::teardown()
{
    if (m_active) {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_tbo);
        glDeleteBuffers(1, &m_nbo);
    }
}
