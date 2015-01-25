#pragma once

#include "Texture.hxx"

#include <memory>
#include <string>

std::unique_ptr<Texture> LoadDDS(const std::string &path);
