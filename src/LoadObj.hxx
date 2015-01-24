#ifndef PHYSICSGAMES_HELLO_OBJLOADER_H_
#define PHYSICSGAMES_HELLO_OBJLOADER_H_

#include <memory>
#include <string>

class Model;

std::unique_ptr<Model> LoadModelFromObjFile(const std::string &path);

#endif // PHYSICSGAMES_HELLO_OBJLOADER_H_
