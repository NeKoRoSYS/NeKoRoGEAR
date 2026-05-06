#include "Core/Rendering/Texture.h"
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

class OpenGLTexture : public Texture {
    uint32_t rendererID;
public:
    OpenGLTexture(const std::string& filepath) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
        
        if (data) {
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
            glGenTextures(1, &rendererID);
            glBindTexture(GL_TEXTURE_2D, rendererID);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << filepath << std::endl;
        }
    }
    
    ~OpenGLTexture() { glDeleteTextures(1, &rendererID); }
    void Bind(uint32_t slot = 0) const override {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, rendererID);
    }
    void Unbind() const override { glBindTexture(GL_TEXTURE_2D, 0); }
};

std::unique_ptr<Texture> Texture::Load(const std::string& filepath) {
    return std::make_unique<OpenGLTexture>(filepath);
}