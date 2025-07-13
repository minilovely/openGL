#include "sb7.h"
#include <math.h>
#include<vmath.h>


class update : public sb7::application
{
public:
	//初始化
	void startup()
	{
		render_Program = compile_shader();

		glCreateVertexArrays(1, &vertex_array_object);
		glBindVertexArray(vertex_array_object);
		glCreateBuffers(1, &buffer);
		aspect = (float)info.windowWidth / (float)info.windowHeight;
		proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

	}
	//渲染函数
	void render(double currentTime)
	{
		static const GLfloat vertex_position[] = 
												{
													// 前面
													-0.25f,  0.25f, -0.25f,
													-0.25f, -0.25f, -0.25f,
													 0.25f, -0.25f, -0.25f,

													 0.25f, -0.25f, -0.25f,
													 0.25f,  0.25f, -0.25f,
													-0.25f,  0.25f, -0.25f,

													// 后面
													-0.25f,  0.25f,  0.25f,
													-0.25f, -0.25f,  0.25f,
													 0.25f, -0.25f,  0.25f,

													 0.25f, -0.25f,  0.25f,
													 0.25f,  0.25f,  0.25f,
													-0.25f,  0.25f,  0.25f,

													// 左面
													-0.25f,  0.25f,  0.25f,
													-0.25f, -0.25f,  0.25f,
													-0.25f, -0.25f, -0.25f,

													-0.25f, -0.25f, -0.25f,
													-0.25f,  0.25f, -0.25f,
													-0.25f,  0.25f,  0.25f,

													// 右面
													 0.25f,  0.25f,  0.25f,
													 0.25f, -0.25f,  0.25f,
													 0.25f, -0.25f, -0.25f,

													 0.25f, -0.25f, -0.25f,
													 0.25f,  0.25f, -0.25f,
													 0.25f,  0.25f,  0.25f,

													 // 下面
													 -0.25f, -0.25f, -0.25f,
													  0.25f, -0.25f, -0.25f,
													  0.25f, -0.25f,  0.25f,

													  0.25f, -0.25f,  0.25f,
													 -0.25f, -0.25f,  0.25f,
													 -0.25f, -0.25f, -0.25f,

													 // 上面
													 -0.25f,  0.25f, -0.25f,
													  0.25f,  0.25f, -0.25f,
													  0.25f,  0.25f,  0.25f,

													  0.25f,  0.25f,  0.25f,
													 -0.25f,  0.25f,  0.25f,
													 -0.25f,  0.25f, -0.25f
												};
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_position), vertex_position, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		float f = (float)currentTime * (float)M_PI * 0.1f;
		mv_matrix = vmath::translate(0.0f, 0.0f, -4.0f)
			* vmath::translate(
				sinf(2.1f * f) * 0.5f,
				cosf(1.7f * f) * 0.5f,
				sinf(1.3f * f) * cosf(1.5f * f) * 2.0f)
			* vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f)
			* vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);



		const GLfloat green[] = { 0.0f,0.0f,0.0f,1.0f };
		glClearBufferfv(GL_COLOR, 0, green);

		glUseProgram(render_Program);

		glUniformMatrix4fv(glGetUniformLocation(render_Program, "mv_matrix"), 1, GL_FALSE, mv_matrix);
		glUniformMatrix4fv(glGetUniformLocation(render_Program, "proj_matrix"), 1, GL_FALSE, proj_matrix);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	//编译·链接着色器
	GLuint compile_shader(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;

		GLuint program;

		static const GLchar* vertex_shader_source[] =
		{
			//"#version 450 core	\n"
			//"					\n"
			//"void main(void)	\n"
			R"(
				#version 450 core
				
				layout(location = 0) in vec4 position;
				out VS_OUT
				{
					vec4 color;
				}vs_out;
				
				uniform mat4 mv_matrix;
				uniform mat4 proj_matrix;
				
				void main(void)
				{
					gl_Position = proj_matrix * mv_matrix * position;
					vs_out.color = position*2.0 + vec4(0.5,0.5,0.5,0.5);
				}
			)"
		};

		static const GLchar* fragment_shader_source[] =
		{
			R"(
				#version 450 core
				
				out vec4 color;
				in VS_OUT
				{
					vec4 color;
				}fs_in; 

				void main(void)
				{
					color = fs_in.color;
				}
			)"
		};
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
		glCompileShader(vertex_shader);


		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
		glCompileShader(fragment_shader);

		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;

	}
	
	//清理程序对象
	void shutdown()
	{
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteBuffers(1, &buffer);
		glDeleteProgram(render_Program);
	}
private:
	GLuint render_Program;
	GLuint vertex_array_object;
	GLuint buffer;

	vmath::mat4 proj_matrix;
	vmath::mat4 mv_matrix;
	float aspect;
};

// 函数实例运行
DECLARE_MAIN(update);
