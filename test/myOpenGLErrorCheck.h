#pragma once
#ifndef MYOPENGLERRORCHECK
#define MYOPENGLERRORCHECK

#include<sb7.h>
#include<vector>
#include<string>
#include<iostream>

//definations are used by printing error
#define RED     "\033[31m"
#define RESET   "\033[0m"
namespace Error
{
	static const GLchar* vertex_source;
	static const GLchar* fragment_source;
	static const GLchar* tessellation_control_source;
	static const GLchar* tessellation_evaluation_source;

	static std::string ver_str;
	static std::string frg_str;
	static std::string tcs_str;
	static std::string tes_str;

	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint tessellation_control_shader;
	GLuint tessellation_evluation_shader;
	GLuint program;

	bool vs_isActive = false;
	bool fs_isActive = false;
	bool tcs_isActive = false;
	bool tes_isActive = false;

	void set_shader_source(std::string ver_src, std::string frg_src);
	void set_shader_source(std::string ver_src, std::string frg_src, std::string tcs_src, std::string tes_src);

	GLuint compile_program();
	void attachShader(GLuint vertex_shader, GLuint fragment_shader,
		GLuint	tessellation_control_shader, GLuint tessellation_evluation_shader);
	
	void check_shader_error(GLuint vertex_shader, GLuint fragment_shader,
							GLuint	tessellation_control_shader,GLuint tessellation_evluation_shader);
	void check_program_error(GLuint program);

}

#endif // !MYOPENGLERRORCHECK
