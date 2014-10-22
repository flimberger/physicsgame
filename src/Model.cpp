#include "Model.hpp"

Model::Model()
  : m_vertices{},
    m_uvCoords{},
    m_normals{}
{}

Model::Model(const std::vector<size_t> faceIndices, const std::vector<glm::vec3> &vertices,
             const std::vector<glm::vec2> &uvCoords, const std::vector<glm::vec3> &normals)
  : m_faceIndices { faceIndices },
    m_vertices { vertices },
    m_uvCoords { uvCoords },
    m_normals { normals }
{}

const std::vector<size_t> &Model::faceIndices() const
{
  return m_faceIndices;
}

const std::vector<glm::vec3> &Model::vertices() const
{
    return m_vertices;
}

const std::vector<glm::vec2> &Model::uvCoords() const
{
    return m_uvCoords;
}

const std::vector<glm::vec3> &Model::normals() const
{
    return m_normals;
}
