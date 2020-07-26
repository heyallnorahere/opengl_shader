#include "include/opengl_shader.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include "glcall.h"
#include "internal.h"
namespace opengl_shader_library {
	static std::string read_file(const std::string& path) {
		using namespace std;
		ifstream file(path);
		stringstream contents;
		string line;
		while (getline(file, line)) contents << line << "\n";
		file.close();
		return contents.str();
	}
	std::string read_shader(const std::string& path) {
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
		std::string path = "shaders/" + name + "/" + (type == GL_VERTEX_SHADER ? "vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : "geometry")) + ".shader";
		std::string source = read_shader(path);
		return create_shader_from_source(source, type);
	}
	void file_shader::create() {
		this->id = glCall(glCreateProgram());
		this->uniforms = new uni(this->id);
		unsigned int gs = NULL;
		auto vs = create_shader(this->name, GL_VERTEX_SHADER);
		auto fs = create_shader(this->name, GL_FRAGMENT_SHADER);
		if (this->geometry_shader) gs = create_shader(this->name, GL_GEOMETRY_SHADER);
		glCall(glAttachShader(this->id, vs));
		glCall(glAttachShader(this->id, fs));
		if (gs) glCall(glAttachShader(this->id, gs));
		glLinkProgram(this->id);
		int success;
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		if (!success) {
			char info_log[512];
			glGetProgramInfoLog(this->id, 512, NULL, info_log);
			std::cout << "error compiling gpu program: " << info_log << std::endl;
		}
		glDeleteShader(vs);
		glDeleteShader(fs);
		if (gs) glDeleteShader(gs);
	}
	file_shader::file_shader(const std::string& name, bool geometry_shader) : name(name), geometry_shader(geometry_shader) {
		this->create();
	}
	void file_shader::reload() {
		delete this->uniforms;
		glDeleteProgram(this->id);
		this->create();
	}
	file_shader::~file_shader() {
		delete this->uniforms;
		glDeleteProgram(this->id);
	}
	void shader::use(shader* s) {
		if (s) { glCall(glUseProgram(s->get_id())); }
		else { glCall(glUseProgram(NULL)); }
	}
	shader::~shader() { }
	uni& file_shader::get_uniforms() {
		return *this->uniforms;
	}
	unsigned int file_shader::get_id() {
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
}