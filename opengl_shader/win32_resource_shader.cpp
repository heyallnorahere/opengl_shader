#ifdef _WIN32
#include <Windows.h>
#include "include/opengl_shader.h"
#include <GL/glew.h>
#include <iostream>
#include "glcall.h"
#include "internal.h"
namespace opengl_shader_library {
	template<typename rtrnt> static rtrnt* load_resource(const char* name, const std::string& type, size_t& length) {
		HRSRC resource = FindResourceA(NULL, name, type.c_str());
		if (!resource) return NULL;
		HGLOBAL global = LoadResource(NULL, resource);
		if (!global) return NULL;
		rtrnt* data = (rtrnt*)LockResource(global);
		if (!data) return NULL;
		length = (size_t)SizeofResource(NULL, resource);
		if (!length) return NULL;
		return data;
	}
	win32_resource_shader::win32_resource_shader(int vertex_id, int fragment_id, const std::string& resource_type, int geometry_id) : vertex_id(vertex_id), fragment_id(fragment_id), resource_type(resource_type), geometry_id(geometry_id) {
		this->create();
	}
	win32_resource_shader::~win32_resource_shader() {
		delete this->uniforms;
		glDeleteShader(this->id);
	}
	uni& win32_resource_shader::get_uniforms() {
		return *this->uniforms;
	}
	unsigned int win32_resource_shader::get_id() {
		return this->id;
	}
	void win32_resource_shader::reload() {
		delete this->uniforms;
		glDeleteShader(this->id);
		this->create();
	}
	static unsigned int create_shader(int id, const std::string& res_type, unsigned int shader_type) {
		size_t length;
		const char* temp = load_resource<const char>(MAKEINTRESOURCEA(id), res_type, length);
		std::string source = std::string(temp, length);
		return create_shader_from_source(source, shader_type);
	}
	void win32_resource_shader::create() {
		unsigned int vs = NULL, fs = NULL, gs = NULL;
		if (this->vertex_id) vs = create_shader(this->vertex_id, this->resource_type, GL_VERTEX_SHADER);
		if (this->fragment_id) fs = create_shader(this->fragment_id, this->resource_type, GL_FRAGMENT_SHADER);
		if (this->geometry_id) gs = create_shader(this->geometry_id, this->resource_type, GL_GEOMETRY_SHADER);
		this->id = glCall(glCreateProgram());
		if (vs) glAttachShader(this->id, vs);
		if (fs) glAttachShader(this->id, fs);
		if (gs) glAttachShader(this->id, gs);
		glLinkProgram(this->id);
		int success;
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);
		if (!success) {
			char info_log[512];
			glGetProgramInfoLog(this->id, 512, NULL, info_log);
			std::cout << "error compiling gpu program: " << info_log << std::endl;
		}
		this->uniforms = new uni(this->id);
		if (vs) glDeleteShader(vs);
		if (fs) glDeleteShader(fs);
		if (gs) glDeleteShader(gs);
	}
}
#endif