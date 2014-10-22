#pragma once

#include <glm/glm.hpp>

#include <vector>

class Model
{
public:
    Model();
    Model(const std::vector<size_t> faceIndices, const std::vector<glm::vec3> &vertices,
          const std::vector<glm::vec2> &uvCoords, const std::vector<glm::vec3> &normals);

    const std::vector<size_t> &faceIndices() const;
    const std::vector<glm::vec3> &vertices() const;
    const std::vector<glm::vec2> &uvCoords() const;
    const std::vector<glm::vec3> &normals() const;

private:
    std::vector<size_t> m_faceIndices;
    // indexed buffer data
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec2> m_uvCoords;
    std::vector<glm::vec3> m_normals;
};
