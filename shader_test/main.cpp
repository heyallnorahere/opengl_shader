#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <opengl_shader.h>
#include <vector>
#include "resource.h"
struct vertex {
	glm::vec3 pos;
};
int main() {
	GLFWwindow* window;
	if (!glfwInit()) return -1;
	window = glfwCreateWindow(800, 600, "idk a test", NULL, NULL);
	glfwMakeContextCurrent(window);
	unsigned int glew_error = glewInit();
	if (glew_error != GLEW_OK) return static_cast<int>(glew_error);
	std::vector<vertex> vertices = {
		{ glm::vec3(-0.5f, -0.5f, 0.f) },
		{ glm::vec3( 0.5f, -0.5f, 0.f) },
		{ glm::vec3( 0.0f,  0.5f, 0.f) },
	};
	unsigned int vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), (void*)offsetof(vertex, pos));
	glEnableVertexAttribArray(0);
	glBindVertexArray(NULL);
	opengl_shader_library::shader* s = new opengl_shader_library::win32_resource_shader(IDR_TEST_VERTEX, IDR_TEST_FRAGMENT);
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		opengl_shader_library::shader::use(s);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(NULL);
		opengl_shader_library::shader::use(NULL);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete s;
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glfwTerminate();
}