#include "LoadDDS.hxx"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

static const uint32_t FOURCC_DXT1 = 0x31545844;
static const uint32_t FOURCC_DXT3 = 0x33545844;
static const uint32_t FOURCC_DXT5 = 0x35545844;

GLuint LoadDDS(const std::string &path)
{
    uint8_t header[124];
    FILE *fp = fopen(path.c_str(), "rb");

    if (fp == nullptr) {
        std::cerr << "Error loading texture: failed to open file \"" << path
                  << "\"." << std::endl;

        return 0;
    }

    char filecode[4];

    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        std::cerr << "Error loading texture: file has incorrect code."
                  << std::endl;
        fclose(fp);

        return 0;
    }
    fread(header, 1, 124, fp);

    uint32_t height = *reinterpret_cast<uint32_t *>(&(header[8]));
    uint32_t width = *reinterpret_cast<uint32_t *>(&(header[12]));
    uint32_t linearSize = *reinterpret_cast<uint32_t *>(&(header[16]));
    uint32_t mipMapCount = *reinterpret_cast<uint32_t *>(&(header[24]));
    uint32_t fourCC = *reinterpret_cast<uint32_t *>(&(header[80]));
    size_t bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    // uint8_t *buffer = new uint8_t[bufsize];
    std::unique_ptr<uint8_t> buffer{new uint8_t[bufsize]};

    fread(buffer.get(), 1, bufsize, fp);
    fclose(fp);

    GLuint format;
    switch (fourCC) {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        return 0;
    }

    GLuint textureId;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    size_t blocksize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    size_t offset = 0;

    for (size_t level = 0; level < mipMapCount && (width || height); ++level) {
        size_t size = ((width + 3) / 4) * ((height + 3) / 4) * blocksize;

        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0,
                               size, buffer.get() + offset);
        offset += size;
        width /= 2;
        height /= 2;
        if (width < 1)
            width = 1;
        if (height < 1)
            height = 1;
    }

    return textureId;
}
