#include "LoadObj.hxx"

#include "Mesh.hxx"

#include <fstream>
#include <iostream>

static const std::size_t FACE_VERTEX{0};
static const std::size_t FACE_UV_COORD{1};
static const std::size_t FACE_NORMAL{2};

static bool ParseTriplet(std::ifstream &file, std::vector<unsigned int> &vec);
static bool ParseFace(std::ifstream &file,
                      std::vector<std::vector<unsigned int>> &faceIndices);

std::unique_ptr<Mesh> LoadMeshFromObjFile(const std::string &path)
{
    std::vector<unsigned int> vertexIdx, uvIdx, normalIdx;
    std::vector<glm::vec3> modelVertices, modelNormals, tmpVertices, tmpNormals;
    std::vector<glm::vec2> modelUVs, tmpUVs;
    std::ifstream file{path};

    if (file.is_open()) {
        while (!file.eof()) {
            std::string strbuf;

            file >> strbuf;
            if (strbuf == "v") {
                glm::vec3 vertex;

                file >> vertex.x;
                file >> vertex.y;
                file >> vertex.z;
                tmpVertices.push_back(vertex);
            } else if (strbuf == "vt") {
                glm::vec2 uv;

                file >> uv.x;
                file >> uv.y;
                tmpUVs.push_back(uv);
            } else if (strbuf == "vn") {
                glm::vec3 normal;

                file >> normal.x;
                file >> normal.y;
                file >> normal.z;
                tmpNormals.push_back(normal);
            } else if (strbuf == "f") {
                std::vector<std::vector<unsigned int>> faceIndices{3};

                if (!ParseFace(file, faceIndices)) {
                    std::cerr << "Error: Failed to parse face." << std::endl;
                    exit(1);
                }
                for (std::size_t i = 0; i < 3; ++i) {
                    vertexIdx.push_back(faceIndices[FACE_VERTEX][i]);
                    uvIdx.push_back(faceIndices[FACE_UV_COORD][i]);
                    normalIdx.push_back(faceIndices[FACE_NORMAL][i]);
                }
            }
        }
    } else {
        std::cerr << "Failed to open file \"" << path << "\"" << std::endl;

        return std::unique_ptr<Mesh>{};
    }
    for (auto i : vertexIdx) {
        glm::vec3 vertex = tmpVertices[i - 1];
        modelVertices.push_back(vertex);
    }
    for (auto i : uvIdx) {
        glm::vec2 uv = tmpUVs[i - 1];
        modelUVs.push_back(uv);
    }
    for (auto i : normalIdx) {
        glm::vec3 normal = tmpNormals[i - 1];
        modelNormals.push_back(normal);
    }

    for (auto vec : modelVertices) {
        std::clog << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
    }

    return std::unique_ptr<Mesh>{
        new Mesh{modelVertices, modelUVs, modelNormals}};
}

static bool ParseTriplet(std::ifstream &file, std::vector<unsigned int> &vec)
{
    unsigned int num;
    char slash;

    file >> num;
    file >> slash;
    if (slash != '/')
        return false;
    vec.push_back(num);
    file >> num;
    file >> slash;
    if (slash != '/')
        return false;
    vec.push_back(num);
    file >> num;
    vec.push_back(num);

    return true;
}

static bool ParseFace(std::ifstream &file,
                      std::vector<std::vector<unsigned int>> &faceIndices)
{
    // 3 components -> 3 iterations
    for (std::size_t i = 0; i < 3; ++i) {
        std::vector<unsigned int> vec;

        if (!ParseTriplet(file, vec))
            return false;
        faceIndices[FACE_VERTEX].push_back(vec[FACE_VERTEX]);
        faceIndices[FACE_UV_COORD].push_back(vec[FACE_UV_COORD]);
        faceIndices[FACE_NORMAL].push_back(vec[FACE_NORMAL]);
    }

    return true;
}
