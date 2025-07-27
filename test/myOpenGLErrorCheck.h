#pragma once
#ifndef MYOPENGLERRORCHECK
#define MYOPENGLERRORCHECK

#include<sb7.h>
#include<vector>
#include<iostream>

//definations are used by printing error
#define RED     "\033[31m"
#define RESET   "\033[0m"
namespace Error
{
	static const GLchar* vertex_source;
	static const GLchar* fragment_source;


	void set_shader_source(std::string ver_src, std::string frg_src);
	GLuint compile_program();
	void check_shader_error(GLuint vertex_shader, GLuint fragment_shader);
	void check_program_error(GLuint program);

}

#endif // !MYOPENGLERRORCHECK
