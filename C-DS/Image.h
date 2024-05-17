#pragma once
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>


namespace image_imp {

	bool loadImage(const char*, GLuint*, int*, int*);
	void SetWindowIcon(GLFWwindow*, const char*);

}



