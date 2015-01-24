#ifndef PHYSICSGAMES_HELLO_OBJLOADER_H_
#define PHYSICSGAMES_HELLO_OBJLOADER_H_

#include <glm/glm.hpp>

#include <string>
#include <vector>

class Model;

Model loadModelFromObjFile(const std::string &path);

#endif // PHYSICSGAMES_HELLO_OBJLOADER_H_
