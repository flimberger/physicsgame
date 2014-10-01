#ifndef PHYSICSGAMES_HELLO_OBJLOADER_H_
#define PHYSICSGAMES_HELLO_OBJLOADER_H_

#include <glm/glm.hpp>

#include <vector>

class Model;

Model LoadModelFromObjFile(const std::string &path);

#endif // PHYSICSGAMES_HELLO_OBJLOADER_H_
