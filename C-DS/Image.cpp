#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

bool image_imp::loadImage(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(filename, &width, &height, &channels, 0);

    if (!imageData) {
        std::cout << "Failed to load image file: " << filename << '\n';
        return false;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);

    stbi_image_free(imageData);

    *out_texture = textureID;
    if (out_width != NULL)
        *out_width = width;
    if (out_height != NULL)
        *out_height = height;
    return true;
}

void image_imp::SetWindowIcon(GLFWwindow* window, const char* iconPath) {
    int width, height, channels;
    unsigned char* data = stbi_load(iconPath, &width, &height, &channels, 0);
    if (data) {
        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = data;
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(data);
    }
    else {
        printf("Failed to load icon: %s\n", iconPath);
    }
}