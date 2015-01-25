#include "Mesh.hxx"

Mesh::Mesh(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvCoords,
           std::vector<glm::vec3> &normals)
    : m_numberOfVertices{vertices.size()},
      /*m_elementBufferId{0},*/ m_normalBufferId{0}, m_uvcBufferId{0},
      m_vertexBufferId{0}
{
    glGenBuffers(1, &m_vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                 &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_uvcBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvcBufferId);
    glBufferData(GL_ARRAY_BUFFER, uvCoords.size() * sizeof(glm::vec2),
                 &uvCoords[0], GL_STATIC_DRAW);

    glGenBuffers(1, &m_normalBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
                 &normals[0], GL_STATIC_DRAW);

    // std::vector<unsigned short> indices;

    // glGenBuffers(1, &m_elementBufferId);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //              indices.size() * sizeof(unsigned short), &indices[0],
    //              GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_normalBufferId);
    glDeleteBuffers(1, &m_uvcBufferId);
    glDeleteBuffers(1, &m_vertexBufferId);
}

void Mesh::Draw() const
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_uvcBufferId);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferId);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_numberOfVertices));
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_elementBufferId);
    // glDrawElements(GL_TRIANGLES, indices.size(),
    // GL_UNSIGNED_SHORT,nullptr);

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

std::size_t Mesh::GetNumberOfVertices() const { return m_numberOfVertices; }
