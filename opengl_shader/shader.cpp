#include "include/opengl_shader.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include "glcall.h"
static std::string read_file(const std::string& path) {
	using namespace std;
	ifstream file(path);
	stringstream contents;
	string line;
	while (getline(file, line)) contents << line << "\n";
	file.close();
	return contents.str();
}
static std::string read_shader(const std::string& path) {
	using namespace std;
	string contents = read_file(path);
	__int64 findpos = -1;
	while ((findpos = (__int64)contents.find("#include", findpos + 1)) != string::npos) {
		size_t first_quote = contents.find("\"", findpos) + 1;
		string fp = "shaders/library/" + contents.substr(first_quote, contents.find("\"", first_quote) - first_quote);
		contents.replace(findpos, contents.find_first_of("\n", findpos) + 1 - findpos, read_shader(fp));
	}
	return contents;
}
static unsigned int create_shader(const std::string name, unsigned int type) {
	unsigned int shader = glCall(glCreateShader(type));
	std::string path = "shaders/" + name + "/" + (type == GL_VERTEX_SHADER ? "vertex" : "fragment") + ".shader";
	std::string source = read_shader(path);
	const char* src = source.c_str();
	glCall(glShaderSource(shader, 1, &src, NULL));
	glCall(glCompileShader(shader));
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << "error compiling " << path << ": " << info_log << std::endl;
	}
	return shader;
}
void shader::create(const std::string& name) {
	this->id = glCall(glCreateProgram());
	this->uniforms = new uni(this->id);
	auto vs = create_shader(name, GL_VERTEX_SHADER);
	auto fs = create_shader(name, GL_FRAGMENT_SHADER);
	glCall(glAttachShader(this->id, vs));
	glCall(glAttachShader(this->id, fs));
	glCall(glLinkProgram(this->id));
	int success;
	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[512];
		glGetProgramInfoLog(this->id, 512, NULL, info_log);
		std::cout << "error compiling gpu program: " << info_log << std::endl;
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
}
shader::shader(const std::string& name) : name(name) {
	this->create(this->name);
}
void shader::reload() {
	delete this->uniforms;
	glDeleteProgram(this->id);
	this->create(this->name);
}
shader::~shader() {
	delete this->uniforms;
	glDeleteProgram(this->id);
}
void shader::use(shader* s) {
	if (s) { glCall(glUseProgram(s->id)); }
	else { glCall(glUseProgram(NULL)); }
}
uni& shader::get_uniforms() {
	return *this->uniforms;
}
unsigned int shader::get_id() {
	return this->id;
}
uni::uni(unsigned int id) : id(id) { }
static int get_location(const std::string& name, unsigned int id) {
	return glGetUniformLocation(id, name.c_str());
}
void uni::_int(const std::string& name, int val) {
	glUniform1i(get_location(name, this->id), val);
}
void uni::_float(const std::string& name, float val) {
	glUniform1f(get_location(name, this->id), val);
}
void uni::vec3(const std::string& name, glm::vec3 val) {
	glUniform3f(get_location(name, this->id), val.x, val.y, val.z);
}
void uni::vec4(const std::string& name, glm::vec4 val) {
	glUniform4f(get_location(name, this->id), val.x, val.y, val.z, val.w);
}
void uni::mat4(const std::string& name, const glm::mat4& val, bool transpose) {
	glUniformMatrix4fv(get_location(name, this->id), 1, transpose, glm::value_ptr(val));
}