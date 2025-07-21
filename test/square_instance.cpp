#include "sb7.h"
#include "vmath.h"
#include <vector>
#include <iostream>

//definations are used by printing error
#define RED     "\033[31m"
#define RESET   "\033[0m"

class Square_instance : public sb7::application
{
public:
	void init()
	{
		static const char title[] = { "square_instance" };
		sb7::application::init();
		memcpy(info.title, title, sizeof(title));
	}
	void startup()
	{
		program = compile_program();
		static const GLfloat square_vertices[] =
		{
			-1.0f, -1.0f, 0.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f,
			 1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f
		};
		static const GLfloat square_color[] =
		{
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f, 1.0f
		};
		static const GLfloat square_position[] =
		{
			-2.0f, -2.0f, 0.0f, 0.0f,
			 2.0f, -2.0f, 0.0f, 0.0f,
			 2.0f,  2.0f, 0.0f, 0.0f,
			-2.0f,  2.0f, 0.0f, 0.0f
		};
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint offset = 0;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices) + sizeof(square_color) + sizeof(square_position), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_vertices), square_vertices);
		offset += sizeof(square_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_color), square_color);
		offset += sizeof(square_color);
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(square_position), square_position);
		offset += sizeof(square_position);
		
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)sizeof(square_vertices));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(square_vertices) + sizeof(square_color)));
		//启用属性的方式两种皆可
		//glEnableVertexArrayAttrib(vao, 0);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);


	}
	void render(double currentTime)
	{
		static const GLfloat black[] = { 0.0f,0.0f,0.0f,0.0f };
		glClearBufferfv(GL_COLOR, 0, black);
		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 4);
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cerr << "OpenGL error: " << err << std::endl;
		}
	}
	void shutdown()
	{
		glDeleteProgram(program);
		glDeleteVertexArrays(1, &vao);
	}

private:
	GLuint program;
	GLuint vao;
	GLuint buffer;
	GLuint compile_program();
	void check_shader_error(GLuint vertex_shader, GLuint fragment_shader);
	void check_program_error(GLuint program);
};

DECLARE_MAIN(Square_instance);

GLuint Square_instance::compile_program()
{
	static const GLchar* vertex_source[] = 
	{ 
		R"(
			#version 410 core
			
			layout(location = 0) in vec4 position;
			layout(location = 1) in vec4 instance_color;
			layout(location = 2) in vec4 instance_position;

			out v2f
			{
				vec4 color;
			}vs_out;
			
			void main()
			{
				gl_Position = (position + instance_position) * vec4(0.25,0.25,1.0,1.0);
				vs_out.color = instance_color;
			};
		)"
	};
	static const GLchar* fragment_source[] =
	{
		R"(
			#version 410 core
			precision highp float;
			in v2f
			{
				vec4 color;
			}vs_out;
			
			out vec4 color;
			
			void main()
			{
				color = vs_out.color;
			}
		)"
	};
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	program = glCreateProgram();
	
	glShaderSource(vertex_shader, 1, vertex_source, NULL);
	glShaderSource(fragment_shader, 1, fragment_source, NULL);
	
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
void Square_instance :: check_shader_error(GLuint vertex_shader, GLuint fragment_shader)
{
	GLint success;
	GLint Log_lenth;
	std::vector<GLchar> Log_info;

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &Log_lenth);
		Log_info.resize(Log_lenth);
		glGetShaderInfoLog(vertex_shader, Log_lenth, NULL, Log_info.data());
		std::cerr << RED << "Vertex Shader Compilation Eorror:\n" << RESET << Log_info.data() << std::endl;
	}
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &Log_lenth);
		Log_info.resize(Log_lenth);
		glGetShaderInfoLog(fragment_shader, Log_lenth, NULL, Log_info.data());
		std::cerr << RED << "Fragment Shader Compilation Eorror:\n" << RESET << Log_info.data() << std::endl;
	}

}
void Square_instance::check_program_error(GLuint program)
{
	GLint success;
	GLint Log_lenth;
	std::vector<GLchar> Log_info;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &Log_lenth);
		Log_info.resize(Log_lenth);
		glGetProgramInfoLog(program, Log_lenth, NULL, Log_info.data());
		std::cerr << RED << "Program Linking Eorror:\n" << RESET << Log_info.data() << std::endl;
	}
}

