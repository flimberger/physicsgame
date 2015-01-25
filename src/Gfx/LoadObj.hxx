#ifndef PHYSICSGAMES_HELLO_OBJLOADER_H_
#define PHYSICSGAMES_HELLO_OBJLOADER_H_

#include <memory>
#include <string>

struct Mesh;

std::unique_ptr<Mesh> LoadMeshFromObjFile(const std::string &path);

#endif // PHYSICSGAMES_HELLO_OBJLOADER_H_
