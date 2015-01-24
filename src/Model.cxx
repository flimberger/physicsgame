#include "Model.hxx"

Model::Model()
  : m_vertices{},
    m_uvCoords{},
    m_normals{}
{}

Model::Model(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvCoords,
		     std::vector<glm::vec3> &normals)
  : m_vertices{vertices},
    m_uvCoords{uvCoords},
    m_normals{normals}
{}

std::vector<glm::vec3> &Model::vertices()
{
    return m_vertices;
}

std::vector<glm::vec2> &Model::uvCoords()
{
    return m_uvCoords;
}

std::vector<glm::vec3> &Model::normals()
{
    return m_normals;
}
