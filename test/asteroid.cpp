#include "sb7.h"
#include "vmath.h"
#include <vector>
#include <iostream>
#include<object.h>
#include"myOpenGLErrorCheck.h"

//definations are used by printing error
#define RED     "\033[31m"
#define RESET   "\033[0m"

enum MODE
{
	MODE_FIRST,
	MODE_MULTIDRAW = 0,
	MODE_SEPARATE_DRAWS,
	MODE_MAX = MODE_SEPARATE_DRAWS
};
enum
{
	DREW_NUMS = 50000
};
class Asteroid : public sb7::application
{
public:
	void init();
	void startup()
	{
		program = compile_program();

		uniforms.time = glGetUniformLocation(program, "time");
		uniforms.proj_matrix = glGetUniformLocation(program, "proj_matrix");
		uniforms.view_matrix = glGetUniformLocation(program, "view_matrix");
		uniforms.viewproj_matrix = glGetUniformLocation(program, "viewproj_matrix");

		object.load("D:/Unity/resourse/OpenGLSuperBible7th/resourse/resourse/sb7code-master/sb7code-master/bin/media/objects/asteroids.sbm");
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, 
					 DREW_NUMS * sizeof(DrawArraysIndirectCommand), NULL, GL_STATIC_DRAW);
		DrawArraysIndirectCommand* cmd = 
			(DrawArraysIndirectCommand*)glMapBufferRange(
												GL_DRAW_INDIRECT_BUFFER,
												0,
												DREW_NUMS * sizeof(DrawArraysIndirectCommand),
												GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		for (int i = 0; i < DREW_NUMS; i++)
		{
			object.get_sub_object_info(i, cmd[i].first, cmd[i].count);
			cmd[i].primCount = 1;
			cmd[i].baseInstance = 1;
		}
		glUnmapBuffer(GL_DRAW_INDIRECT_BUFFER);
		
		glBindVertexArray(object.get_vao());
		if (mode == MODE_MULTIDRAW)
		{
			glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, DREW_NUMS, 0);
		}
		else if (mode == MODE_SEPARATE_DRAWS)
		{
			for (int j = 0; j < DREW_NUMS; j++)
			{
				GLuint first, count;
				object.get_sub_object_info(j % object.get_sub_object_count(), first, count);
				glDrawArraysInstancedBaseInstance(GL_TRIANGLES,
					first,
					count,
					1, j);
			}
		}


	}
	void render(double t)
	{
		static const GLfloat black[] = { 0.0f,0.0f,0.0f,0.0f };
		glClearBufferfv(GL_COLOR, 0, black);
		glUseProgram(program);
		const GLfloat time = float(t);
		const vmath::mat4 view_matrix = vmath::lookat(
							vmath::vec3(100.0f * cosf(t * 0.023f), 100.0f * cosf(t * 0.023f), 300.0f * sinf(t * 0.037f) - 600.0f),
							vmath::vec3(0.0f, 0.0f, 260.0f),
							vmath::normalize(vmath::vec3(0.1f - cosf(t * 0.1f) * 0.3f, 1.0f, 0.0f)));
		const vmath::mat4 proj_matrix = vmath::perspective(50.0f,
			(float)info.windowWidth / (float)info.windowHeight,
			1.0f,
			2000.0f);
		//uniform变量传递
		glUniform1f(uniforms.time, time);
		glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, view_matrix);
		glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);
		glUniformMatrix4fv(uniforms.viewproj_matrix, 1, GL_FALSE, proj_matrix * view_matrix);


		glDrawArrays(GL_POINTS, 0, 1);
	}
	void shutdown();
private:
		GLuint program;
		GLuint buffer;
		MODE mode;
		sb7::object object;

		struct
		{
			GLint time;
			GLint view_matrix;
			GLint proj_matrix;
			GLint viewproj_matrix;
		} uniforms;
		struct DrawArraysIndirectCommand
		{
			GLuint  count;
			GLuint  primCount;
			GLuint  first;
			GLuint  baseInstance;
		};

		GLuint compile_program();
		void check_shader_error(GLuint vertex_shader, GLuint fragment_shader);
		void check_program_error(GLuint program);
		void check_OpenGL_error(GLenum err);

protected:
};

	DECLARE_MAIN(Asteroid);

	GLuint Asteroid::compile_program()
	{
		static const GLchar* vertex_source[] =
		{
			R"(
				#version 410 core

				layout (location = 0) in vec3 position_3;
				layout (location = 1) in vec3 normal;

				layout (location = 10) in uint draw_id;

				out VS_OUT
				{
					vec3 normal;
					vec4 color;
				} vs_out;

				uniform float time = 0.0;

				uniform mat4 view_matrix;
				uniform mat4 proj_matrix;
				uniform mat4 viewproj_matrix;

				const vec4 color0 = vec4(0.29, 0.21, 0.18, 1.0);
				const vec4 color1 = vec4(0.58, 0.55, 0.51, 1.0);

				void main(void)
				{
					vec4 position = vec4(position_3, 1.0);
					mat4 m1;
					mat4 m2;
					mat4 m;
					float t = time * 0.1;
					float f = float(draw_id) / 30.0;

					float st = sin(t * 0.5 + f * 5.0);
					float ct = cos(t * 0.5 + f * 5.0);
					//fract()获取小数部分
					float j = fract(f);
					float d = cos(j * 3.14159);

					// Rotate around Y
					m[0] = vec4(ct, 0.0, st, 0.0);
					m[1] = vec4(0.0, 1.0, 0.0, 0.0);
					m[2] = vec4(-st, 0.0, ct, 0.0);
					m[3] = vec4(0.0, 0.0, 0.0, 1.0);

					// Translate in the XZ plane
					m1[0] = vec4(1.0, 0.0, 0.0, 0.0);
					m1[1] = vec4(0.0, 1.0, 0.0, 0.0);
					m1[2] = vec4(0.0, 0.0, 1.0, 0.0);
					m1[3] = vec4(260.0 + 30.0 * d, 5.0 * sin(f * 123.123), 0.0, 1.0);

					m = m * m1;

					// Rotate around X
					st = sin(t * 2.1 * (600.0 + f) * 0.01);
					ct = cos(t * 2.1 * (600.0 + f) * 0.01);

					m1[0] = vec4(ct, st, 0.0, 0.0);
					m1[1] = vec4(-st, ct, 0.0, 0.0);
					m1[2] = vec4(0.0, 0.0, 1.0, 0.0);
					m1[3] = vec4(0.0, 0.0, 0.0, 1.0);

					m = m * m1;

					// Rotate around Z
					st = sin(t * 1.7 * (700.0 + f) * 0.01);
					ct = cos(t * 1.7 * (700.0 + f) * 0.01);

					m1[0] = vec4(1.0, 0.0, 0.0, 0.0);
					m1[1] = vec4(0.0, ct, st, 0.0);
					m1[2] = vec4(0.0, -st, ct, 0.0);
					m1[3] = vec4(0.0, 0.0, 0.0, 1.0);

					m = m * m1;

					// Non-uniform scale
					float f1 = 0.65 + cos(f * 1.1) * 0.2;
					float f2 = 0.65 + cos(f * 1.1) * 0.2;
					float f3 = 0.65 + cos(f * 1.3) * 0.2;

					m1[0] = vec4(f1, 0.0, 0.0, 0.0);
					m1[1] = vec4(0.0, f2, 0.0, 0.0);
					m1[2] = vec4(0.0, 0.0, f3, 0.0);
					m1[3] = vec4(0.0, 0.0, 0.0, 1.0);

					m = m * m1;

					gl_Position = viewproj_matrix * m * position;
					vs_out.normal = mat3(view_matrix * m) * normal;
					vs_out.color = mix(color0, color1, fract(j * 313.431));
				}

		)"
		};
		static const GLchar* fragment_source[] =
		{
			R"(
				#version 410 core

				out vec4 color;

				in VS_OUT
				{
					vec3 normal;
					vec4 color;
				} fs_in;

				void main(void)
				{
					vec3 N = normalize(fs_in.normal);
					//abs()函数返回绝对值
					color = fs_in.color * abs(N.z);
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
	void Asteroid::init()
	{
		static const char title[] = { "Asteroid" };
		sb7::application::init();
		memcpy(info.title, title, sizeof(title));
	}
	void Asteroid::shutdown()
	{
		glDeleteProgram(program);
	}
	void Asteroid::check_shader_error(GLuint vertex_shader, GLuint fragment_shader)
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
	void Asteroid::check_program_error(GLuint program)
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
			std::cerr << RED << "Program Linking Eorror:\n" << RESET << Log_info.data() << std::endl;
		}
	}
	void Asteroid::check_OpenGL_error(GLenum err)
	{
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << err << std::endl;
		}
	}
