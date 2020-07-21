#pragma once
#include <string>
#include <glm/glm.hpp>
class uni {
public:
	uni(unsigned int id);
	void _int(const std::string& name, int val);
	void _float(const std::string& name, float val);
	void vec3(const std::string& name, glm::vec3 val);
	void vec4(const std::string& name, glm::vec4 val);
	void mat4(const std::string& name, const glm::mat4& val, bool transpose = false);
private:
	unsigned int id;
};
class shader {
public:
	shader(const std::string& name, bool geometry_shader = false);
	~shader();
	static void use(shader*);
	uni& get_uniforms();
	unsigned int get_id();
	void reload();
private:
	std::string name;
	void create(const std::string& name);
	unsigned int id;
	uni* uniforms;
	bool geometry_shader;
};