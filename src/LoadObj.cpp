#include "LoadObj.hpp"

#include "Model.hpp"

#include <fstream>
#include <iostream>

struct IndexTriplet
{
    unsigned int vertexIndex;
    unsigned int uvIndex;
    unsigned int normalIndex;
};

static bool operator==(const IndexTriplet &left, const IndexTriplet &right);

static bool parseTriplet(std::ifstream &file, IndexTriplet &indexTriplet);
static bool parseFace(std::ifstream &file, std::vector<size_t> &faceIndices,
                      std::vector<IndexTriplet> &faceTriplets);

Model
loadModelFromObjFile(const std::string &path)
{
    std::vector<IndexTriplet> faceTriplets;
    std::vector<size_t> faceIndices;
    std::vector<glm::vec3> modelVertices, modelNormals, tmpVertices, tmpNormals;
    std::vector<glm::vec2> modelUVs, tmpUVs;
    std::ifstream file { path };

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
                if (!parseFace(file, faceIndices, faceTriplets)) {
                    std::cerr << "Error: Failed to parse face." << std::endl;
                    exit(1);
                }
            }
        }
    }

    for (auto &t : faceTriplets) {
        modelVertices.push_back(tmpVertices[t.vertexIndex - 1]);
        modelUVs.push_back(tmpUVs[t.uvIndex - 1]);
        modelNormals.push_back(tmpNormals[t.normalIndex - 1]);
    }

    for (auto vec : modelVertices) {
        std::clog << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
    }

    return Model { faceIndices, modelVertices, modelUVs, modelNormals };
}

static bool operator==(const IndexTriplet &left, const IndexTriplet &right)
{
    return (left.vertexIndex == right.vertexIndex)
        && (left.uvIndex == right.uvIndex)
        && (left.normalIndex == right.normalIndex);
}

static bool
parseTriplet(std::ifstream &file, IndexTriplet &indexTriplet)
{
    char slash;

    file >> indexTriplet.vertexIndex;
    file >> slash;
    if (slash != '/')
        return false;
    file >> indexTriplet.uvIndex;
    file >> slash;
    if (slash != '/')
        return false;
    file >> indexTriplet.normalIndex;

    return true;
}

static bool
parseFace(std::ifstream &file, std::vector<size_t> &faceIndices,
          std::vector<IndexTriplet> &faceTriplets)
{
    // 3 components -> 3 iterations
    for (size_t i = 0; i < 3; ++i) {
        IndexTriplet triplet;

        if (!parseTriplet(file, triplet))
            return false;

        size_t faceIndex = 0;
        for (; faceIndex < faceTriplets.size(); ++faceIndex) {
            if (triplet == faceTriplets[faceIndex]) {
                goto push_index;
            }
        }
        faceTriplets.push_back(triplet);
push_index:
        faceIndices.push_back(faceIndex);
    }

    return true;
}
