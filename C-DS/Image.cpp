#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>


bool image_imp::loadImage(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int width, height, channels;
    unsigned char* img = stbi_load(filename, &width, &height, &channels, 0);
    if (!img) {
        std::cout << "Failed to load image file: " << filename << '\n';
        return false;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);

    *out_texture = tex;
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