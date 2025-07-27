#include "myOpenGLErrorCheck.h"
#include<string>
namespace Error
{
	void set_shader_source(std::string ver_src, std::string frg_src)
	{
		static std::string ver_str = ver_src; 
		static std::string frg_str = frg_src;
		vertex_source = ver_str.c_str();
		fragment_source = frg_str.c_str();
	}
	GLuint compile_program()
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		program = glCreateProgram();

		glShaderSource(vertex_shader, 1, &vertex_source, NULL);
		glShaderSource(fragment_shader, 1, &fragment_source, NULL);

		glCompileShader(vertex_shader);
		glCompileShader(fragment_shader);

		check_shader_error(vertex_shader, fragment_shader);

		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		glLinkProgram(program);

		check_program_error(program);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;
	}
	void check_shader_error(GLuint vertex_shader, GLuint fragment_shader)
	{
		GLint success;
		GLint Log_length;
		std::vector<GLchar> Log_info;

		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &Log_length);
			Log_info.resize(Log_length);
			glGetShaderInfoLog(vertex_shader, Log_length, NULL, Log_info.data());
			std::cerr << RED << "Vertex Shader Compilation Error:\n" << RESET << Log_info.data() << std::endl;
		}
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &Log_length);
			Log_info.resize(Log_length);
			glGetShaderInfoLog(fragment_shader, Log_length, NULL, Log_info.data());
			std::cerr << RED << "Fragment Shader Compilation Error:\n" << RESET << Log_info.data() << std::endl;
		}

	}
	void check_program_error(GLuint program)
	{
		GLint success;
		GLint Log_length;
		std::vector<GLchar> Log_info;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &Log_length);
			Log_info.resize(Log_length);
			glGetProgramInfoLog(program, Log_length, NULL, Log_info.data());
			std::cerr << RED << "Program Linking Error:\n" << RESET << Log_info.data() << std::endl;
		}
	}

}

