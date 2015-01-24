#pragma once

#include <glm/glm.hpp>

#include <vector>

class Model
{
  public:
    Model();
    Model(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvCoords,
          std::vector<glm::vec3> &normals);

    std::vector<glm::vec3> &GetVertices();
    std::vector<glm::vec2> &GetUvCoords();
    std::vector<glm::vec3> &GetNormals();

  private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvCoords;
    std::vector<glm::vec3> m_normals;
};
