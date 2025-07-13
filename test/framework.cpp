#include "sb7.h"
#include <math.h>


class my_application : public sb7::application
{
public:
	void startup() 
	{
		render_Program = compile_shader();
		glCreateVertexArrays(1, &vertex_array_object);

		glBindVertexArray(vertex_array_object);
	}
	//渲染函数
	void render(double currentTime)
	{
		//颜色，随着时间而改变。分别是rgba，红绿蓝和透明度
		const GLfloat color[] = {	0.0f,
									0.2f, 
									0.0f, 
									1.0f };
		//清除第一个参数指定的缓存为第三个参数的值
		glClearBufferfv(GL_COLOR, 0, color);
		glUseProgram(render_Program);
		//glPointSize(40.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
				
				void main(void)
				{
					const vec4 vertices[3] = vec4[3](
												vec4(0.25,-0.25,0.5,1.0),
												vec4(-0.25,-0.25,0.5,1.0),
												vec4(0.25,0.25,0.5,1.0)
											);
					gl_Position = vertices[gl_VertexID];
				}
			)"
		};

		static const GLchar* fragment_shader_source[] =
		{
			R"(
				#version 450 core
				
				out vec4 color;
				
				void main(void)
				{
					color = vec4(0.0,0.8,1.0,1.0);
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
		glDeleteProgram(render_Program);
	}
private:
	GLuint render_Program;
	GLuint vertex_array_object;
};

// 函数实例运行
DECLARE_MAIN(my_application);
